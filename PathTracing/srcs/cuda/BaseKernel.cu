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

#include "Compute.hpp"

// __global__ void clearKernel(uchar4* buffer, int width, int height, uchar4 color);

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

void Compute::update(uchar4* devPtr, const SceneData& scene) {
	// 🔹 Lancement kernel
	dim3 block(16,16);
	dim3 grid((_width + block.x - 1)/block.x,
	          (_height + block.y - 1)/block.y);

	// uchar4 color = make_uchar4(70, 130, 180, 255); // bleu doux
	clearKernel<<<grid, block>>>(devPtr, _width, _height, 0.5f);

	// Unneccessary here because cudaGraphicsUnmapResources() already do it
	// cudaDeviceSynchronize();
}
