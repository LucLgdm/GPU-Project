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

__global__ void clearKernel(uchar4* buffer, int width, int height, uchar4 color) {
	int x = blockIdx.x * blockDim.x + threadIdx.x;
	int y = blockIdx.y * blockDim.y + threadIdx.y;

	if (x >= width || y >= height) return;

	int i = y * width + x;
	buffer[i] = color;
}

void Compute::update(uchar4* devPtr) {
	// 🔹 Lancement kernel
	dim3 block(16,16);
	dim3 grid((_width + block.x - 1)/block.x,
	          (_height + block.y - 1)/block.y);

	uchar4 color = make_uchar4(70, 130, 180, 255); // bleu doux

	clearKernel<<<grid, block>>>(devPtr, _width, _height, color);

	// Unneccessary here because cudaGraphicsUnmapResources() already do it
	// cudaDeviceSynchronize();
}
