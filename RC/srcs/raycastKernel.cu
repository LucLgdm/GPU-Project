/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   raycastKernel.cu             	                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/10 12:59:15 by lde-merc          #+#    #+#             */
/*   Updated: 2026/03/10 12:55:14 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Raycaster.hpp"

__global__ void raycastKernel(uchar4*, Camera, char*, int, int, int, int, uchar4*, uchar4*, int, int);
__global__ void floorKernel(uchar4*, Camera, uchar4*, uchar4*, int, int, int, int);

/********************************************************************************
 * Map data and texture transfer to GPU
 ********************************************************************************/

void Raycaster::sendMapGpu() {
	_flatMap.reserve(_mapWidth * _mapHeight);
	for (const auto &row : _map) {
		for (char c : row)
			_flatMap.push_back(c);
		// Compléter jusqu'à _mapWidth
		for (int i = row.size(); i < _mapWidth; i++)
			_flatMap.push_back(' ');
	}

	cudaMalloc(&_devMap, _flatMap.size() * sizeof(char));
	cudaMemcpy(_devMap, _flatMap.data(), _flatMap.size() * sizeof(char), cudaMemcpyHostToDevice);
}

void Raycaster::loadTexture(const char* path, uchar4** devTex, int& texW, int& texH) {
	int channels;
	unsigned char* data = stbi_load(path, &texW, &texH, &channels, 4);
	if (!data)
		throw inputError("Failed to load texture: " + std::string(path));
	
	cudaMalloc(devTex, texW * texH * sizeof(uchar4));
	cudaMemcpy(*devTex, data, texW * texH * sizeof(uchar4), cudaMemcpyHostToDevice);
	
	stbi_image_free(data);
}

struct Ray {
	float dirX;
	float dirY;
};

struct DDA {
	int mapX;
	int mapY;

	float sideDistX;
	float sideDistY;

	float deltaDistX;
	float deltaDistY;

	int stepX;
	int stepY;

	int side;
};

struct WallHit {
	float perpDist;
	int lineHeight;
	int drawStart;
	int drawEnd;
};

/*************************************************************************************
 * Main update loop and CUDA kernel launch
 *************************************************************************************/

void Raycaster::update(uchar4 *devPtr) {
	// This function will be called every frame to update the raycasting logic.
	// It will launch the CUDA kernel to perform the raycasting calculations on the GPU.
	dim3 blocksize(16, 16);
	dim3 gridSize((_screenWidth + blocksize.x - 1) / blocksize.x,
				  (_screenHeight + blocksize.y - 1) / blocksize.y);

	floorKernel<<<gridSize, blocksize>>>(devPtr, _camera, _devTexFloor, _devTexCeil,
									_texWidth, _texHeight, _screenWidth, _screenHeight);
	cudaDeviceSynchronize();

	cudaError_t err = cudaGetLastError();
	if (err != cudaSuccess)
		throw cuda_Error(cudaGetErrorString(err));

	// Raycasting kernel launch
	int nbThreads = _screenWidth;
	int blockSize = 256;
	int nbBlocks = (nbThreads + blockSize - 1) / blockSize;

	raycastKernel<<<nbBlocks, blockSize>>>(devPtr, _camera, _devMap, _mapWidth, _mapHeight,
												_screenWidth, _screenHeight,
												_devTexNS, _devTexEW, _texWidth, _texHeight);
	cudaDeviceSynchronize();

	err = cudaGetLastError();
	if (err != cudaSuccess)
		throw cuda_Error(cudaGetErrorString(err));
}

__device__ Ray computeRay(int x, int screenWidth, Camera cam) {
	float cameraX = 2.0f * x / screenWidth - 1.0f;

	Ray ray;
	ray.dirX = cam.dirX + cam.planeX * cameraX;
	ray.dirY = cam.dirY + cam.planeY * cameraX;

	return ray;
}

__device__ DDA initDDA(Camera cam, Ray ray) {
	DDA d;

	d.mapX = (int)cam.x;
	d.mapY = (int)cam.y;

	d.deltaDistX = fabsf(1.0f / ray.dirX);
	d.deltaDistY = fabsf(1.0f / ray.dirY);

	if (ray.dirX < 0) {
		d.stepX = -1;
		d.sideDistX = (cam.x - d.mapX) * d.deltaDistX;
	} else {
		d.stepX = 1;
		d.sideDistX = (d.mapX + 1.0f - cam.x) * d.deltaDistX;
	}

	if (ray.dirY < 0) {
		d.stepY = -1;
		d.sideDistY = (cam.y - d.mapY) * d.deltaDistY;
	} else {
		d.stepY = 1;
		d.sideDistY = (d.mapY + 1.0f - cam.y) * d.deltaDistY;
	}

	return d;
}

__device__ float performDDA(DDA& d, Ray ray, char* map, int mapWidth, int mapHeight) {
	int hit = 0;

	while (!hit) {
		if (d.sideDistX < d.sideDistY) {
			d.sideDistX += d.deltaDistX;
			d.mapX += d.stepX;
			d.side = 0;
		} else {
			d.sideDistY += d.deltaDistY;
			d.mapY += d.stepY;
			d.side = 1;
		}

		if (d.mapX >= 0 && d.mapX < mapWidth &&
			d.mapY >= 0 && d.mapY < mapHeight &&
			map[d.mapY * mapWidth + d.mapX] == '1')
			hit = 1;
	}

	if (d.side == 0)
		return d.sideDistX - d.deltaDistX;
	else
		return d.sideDistY - d.deltaDistY;
}

__device__ WallHit computeWall(float perpDist, int screenHeight) {
	WallHit w;

	w.perpDist = perpDist;
	w.lineHeight = (int)(screenHeight / perpDist);

	w.drawStart = screenHeight / 2 - w.lineHeight / 2;
	w.drawEnd   = screenHeight / 2 + w.lineHeight / 2;

	if (w.drawStart < 0) w.drawStart = 0;
	if (w.drawEnd >= screenHeight) w.drawEnd = screenHeight - 1;

	return w;
}

__device__ void drawColumn( uchar4* pbo, int x, int screenWidth, int screenHeight,
							WallHit wall, DDA d, Ray ray, Camera cam,
							uchar4* texNS, uchar4* texEW, int texW, int texH)
{
	float brightness = __saturatef(1.0f / (wall.perpDist * 0.4f));

	float wallX;
	if (d.side == 0)
		wallX = cam.y + wall.perpDist * ray.dirY;
	else
		wallX = cam.x + wall.perpDist * ray.dirX;
	wallX -= floor(wallX);

	int texX = (int)(wallX * texW);
	if (texX < 0) texX = 0;
	if (texX >= texW) texX = texW - 1;

	// Draw walls
	for (int y = wall.drawStart; y <= wall.drawEnd; y++) {
		uchar4 color;
		int texY = (int)((y - screenHeight/2 + wall.lineHeight/2) * texH / wall.lineHeight);
		if (texY < 0) texY = 0;
		if (texY >= texH) texY = texH - 1;

		uchar4* tex = (d.side == 0) ? texNS : texEW;
		uchar4 t = tex[texY * texW + texX];

		color = make_uchar4(
			t.x * brightness,
			t.y * brightness,
			t.z * brightness,
			255);
		pbo[y * screenWidth + x] = color;
	}

// 	// unsigned char intensity = (unsigned char)(brightness * 255);

// 	/* Wall with colors */
// 	/*
// 	for (int y = 0; y < screenHeight; y++) {
// 		uchar4 color;
// 		if (y < drawStart) {
// 			color = make_uchar4(50, 50, 50, 255);   // floor
// 		} else if (y <= drawEnd) {
// 			if (side == 1)
// 				color = make_uchar4(intensity, 0, 0, 255); // rouge qui s'assombrit
// 			else
// 				color = make_uchar4(0, 0, intensity, 255); // bleu qui s'assombrit
// 		} else {
// 			color = make_uchar4(50, 50, 175, 255); // ceiling
// 		}
// 		pbo[y * screenWidth + idx] = color;
// 	}
// 	*/
}


__global__ void raycastKernel(uchar4* pbo, Camera cam,
								char* map, int mapWidth, int mapHeight, int screenWidth, int screenHeight,
								uchar4* texNS, uchar4* texEW, int texW, int texH)
{
	int x = blockIdx.x * blockDim.x + threadIdx.x;
	if (x >= screenWidth) return;

	Ray ray = computeRay(x, screenWidth, cam);

	DDA dda = initDDA(cam, ray);

	float perpDist = performDDA(
		dda, ray, map, mapWidth, mapHeight);

	WallHit wall = computeWall(perpDist, screenHeight);

	drawColumn(pbo, x, screenWidth, screenHeight, wall, dda, ray, cam, texNS, texEW, texW, texH);
}
