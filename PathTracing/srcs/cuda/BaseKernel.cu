/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BaseKernel.cu                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/25 18:40:30 by lde-merc          #+#    #+#             */
/*   Updated: 2026/03/25 18:40:48 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Ray.cuh"
#include "Compute.hpp"
#include "CameraData.hpp"

struct GPUCameraData {
	float3 origin;
	float3 lowerLeftCorner;
	float3 horizontal;
	float3 vertical;
};

__constant__ GPUCameraData d_camera;

__global__ void clearKernel(uchar4* buffer, int width, int height, float time) {
	int x = blockIdx.x * blockDim.x + threadIdx.x;
	int y = blockIdx.y * blockDim.y + threadIdx.y;

	if (x >= width || y >= height) return;

	int i = y * width + x;
	float fx = (float)x / width;
	float fy = (float)y / height;

	float wave = sinf(fx * 10.0f + time) * cosf(fy * 10.0f + time);

	float r = 0.5f + 0.5f * wave;
	float g = 0.5f + 0.5f * sinf(time);
	float b = 0.5f + 0.5f * cosf(time);

    uchar4 color = make_uchar4(
        (unsigned char)(r * 255.0f),
        (unsigned char)(g * 255.0f),
        (unsigned char)(b * 255.0f),
        255
    );
	buffer[i] = color;
}

__device__ Ray generateRay(GPUCameraData cam, float u, float v) {
	Ray ray;

	ray.origin = cam.origin;
	ray.dir = cam.lowerLeftCorner + cam.horizontal * u + cam.vertical * v - ray.origin;
	ray.dir = normalize(ray.dir);

	return ray;
}

__global__ void pathTraceKernel(uchar4* fb, int width, int height,
								const Triangle* triangles, int triCount) {

	// x and y are the pixel coordinate
	int x = blockIdx.x * blockDim.x + threadIdx.x;
	int y = blockIdx.y * blockDim.y + threadIdx.y;

	if (x >= width || y >= height) return;

	int idx = x + y * width;
	float u = (float)x / width;
	float v = (float)y / height;

	Ray ray = generateRay(d_camera, u, v);
	HitRecord hit = intersectScene(ray, triangles, triCount);

	if (hit.hit)
		fb[idx] = toRGBA8(hit.normal * 0.5f + 0.5f);
	else
		fb[idx] = toRGBA8(make_float3(0.0f, 0.0f, 0.6f));
}

// 🔹 Lancement kernel
void Compute::update(uchar4* devPtr, const SceneData& scene) {
	dim3 block(16,16);
	dim3 grid((_width + block.x - 1)/block.x,
	          (_height + block.y - 1)/block.y);

	pathTraceKernel<<<grid, block>>>(devPtr, _width, _height, scene.triangles, scene.triangleCount);
}

void uploadCamera(const CameraData& cam) {
	GPUCameraData gpuCam;

	gpuCam.origin = make_float3(cam.origin[0], cam.origin[1], cam.origin[2]);
	gpuCam.lowerLeftCorner = make_float3(cam.lowerLeftCorner[0], cam.lowerLeftCorner[1], cam.lowerLeftCorner[2]);
	gpuCam.horizontal = make_float3(cam.horizontal[0], cam.horizontal[1], cam.horizontal[2]);
	gpuCam.vertical = make_float3(cam.vertical[0], cam.vertical[1], cam.vertical[2]);

    cudaMemcpyToSymbol(d_camera, &gpuCam, sizeof(CameraData));
}
