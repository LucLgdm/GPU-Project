/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Kernel.cu                   	                    :+:      :+:    :+:   */
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

/*************************************************************************************
 * Main update loop and CUDA kernel launch
 *************************************************************************************/

void Raycaster::update(uchar4 *devPtr) {
	// This function will be called every frame to update the raycasting logic.
	// It will launch the CUDA kernel to perform the raycasting calculations on the GPU.
	int nbThreads = _screenWidth;
	int blockSize = 256;
	int nbBlocks = (nbThreads + blockSize - 1) / blockSize;

	raycastKernel<<<nbBlocks, blockSize>>>(devPtr, _camera, _devMap,
												_mapWidth, _mapHeight,
												_screenWidth, _screenHeight,
												_devTexNS, _devTexEW, _texWidth, _texHeight);
	cudaDeviceSynchronize();

	cudaError_t err = cudaGetLastError();
	if (err != cudaSuccess)
		throw cuda_Error(cudaGetErrorString(err));
}

__global__ void raycastKernel(uchar4* pbo, Camera cam, char* map, 
							int mapWidth, int mapHeight,
							int screenWidth, int screenHeight,
							uchar4* texNS, uchar4* texEW, int texW, int texH)
{
	int idx = blockIdx.x * blockDim.x + threadIdx.x;
	if (idx >= screenWidth) return;

	// Calculate ray position and direction
	float cameraX = 2.0f * idx / screenWidth - 1.0f; // Coordinate in camera space : [-1, 1]
	float rayDirX = cam.dirX + cam.planeX * cameraX;
	float rayDirY = cam.dirY + cam.planeY * cameraX;
	
	// Map position
	int mapX = (int)cam.x;
	int mapY = (int)cam.y;

	// Length of ray from current position to next x or y-side
	float sideDistX;
	float sideDistY;

	// Length of ray from one x or y-side to next x or y-side
	// if rayDirX is 0, deltaDistX will be inf, so we handle that case
	float deltaDistX = fabsf(1.0f / rayDirX);
	float deltaDistY = fabsf(1.0f / rayDirY);
	float perpWallDist;

	// Direction to step in x and y
	int stepX;
	int stepY;
	int hit = 0; // Was there a wall hit?
	int side; // Was a NS or a EW wall hit?
	
	// Calculate step and initial sideDist
	if (rayDirX < 0) {
		stepX = -1;
		sideDistX = (cam.x - mapX) * deltaDistX;
	} else {
		stepX = 1;
		sideDistX = (mapX + 1.0f - cam.x) * deltaDistX;
	}
	if (rayDirY < 0) {
		stepY = -1;
		sideDistY = (cam.y - mapY) * deltaDistY;
	} else {
		stepY = 1;
		sideDistY = (mapY + 1.0f - cam.y) * deltaDistY;
	}

	// Perform DDA (Digital Differential Analyzer)
	while (hit == 0) {
		// Jump to next map square, either in x-direction, or in y-direction
		if (sideDistX < sideDistY) {
			sideDistX += deltaDistX;
			mapX += stepX;
			side = 0;
		} else {
			sideDistY += deltaDistY;
			mapY += stepY;
			side = 1;
		}
		// Check if ray has hit a wall
		if (mapY >= 0 && mapY < mapHeight && mapX >= 0 && mapX < mapWidth
			&& map[mapY * mapWidth + mapX] == '1') hit = 1;
	}

	// Calculate distance projected on camera direction (Euclidean distance will give fisheye effect!)
	// The last addition of deltaDist went one step too far, so we remove it
	if (side == 0)
		perpWallDist = sideDistX - deltaDistX;
	else
		perpWallDist = sideDistY - deltaDistY;

	int lineHeight = (int)(screenHeight / perpWallDist);

	int drawStart = screenHeight / 2 - lineHeight / 2;
	int drawEnd   = screenHeight / 2 + lineHeight / 2;

	// Clamp values to screen boundaries
	if (drawStart < 0) drawStart = 0;
	if (drawEnd >= screenHeight) drawEnd = screenHeight - 1;

	// 0.0 to 1.0, speed factor for brightness based on distance
	float brightness = __saturatef(1.0f / (perpWallDist * 0.4f)); 
	// unsigned char intensity = (unsigned char)(brightness * 255);

	/* Wall with colors */
	/*
	for (int y = 0; y < screenHeight; y++) {
		uchar4 color;
		if (y < drawStart) {
			color = make_uchar4(50, 50, 50, 255);   // floor
		} else if (y <= drawEnd) {
			if (side == 1)
				color = make_uchar4(intensity, 0, 0, 255); // rouge qui s'assombrit
			else
				color = make_uchar4(0, 0, intensity, 255); // bleu qui s'assombrit
		} else {
			color = make_uchar4(50, 50, 175, 255); // ceiling
		}
		pbo[y * screenWidth + idx] = color;
	}
	*/

	/* Wall with textures */
	float wallX;
	if (side == 0)
		wallX = cam.y + perpWallDist * rayDirY;
	else
		wallX = cam.x + perpWallDist * rayDirX;
	wallX -= floor(wallX);

	int texX = (int)(wallX * texW);
	if (texX < 0) texX = 0;
	if (texX >= texW) texX = texW - 1;
	for (int y = 0; y < screenHeight; y++) {
		uchar4 color;
		if (y < drawStart) {
			color = make_uchar4(50, 50, 50, 255);   // floor
		} else if (y <= drawEnd) {
			int texY = (int)((y - screenHeight / 2 + lineHeight / 2) * texH / lineHeight);
			if (texY < 0) texY = 0;
			if (texY >= texH) texY = texH - 1;
			uchar4* tex = (side == 0) ? texNS : texEW;
			// color = tex[texY * texW + texX];
			uchar4 texColor = tex[texY * texW + texX];
			color = make_uchar4(
				(unsigned char)(texColor.x * brightness),
				(unsigned char)(texColor.y * brightness),
				(unsigned char)(texColor.z * brightness),
				255
			);
		} else {
			color = make_uchar4(50, 50, 175, 255); // ceiling
		}
		pbo[y * screenWidth + idx] = color;
	}
}
