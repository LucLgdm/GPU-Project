/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MathUtils.cuh                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/31 12:45:59 by lde-merc          #+#    #+#             */
/*   Updated: 2026/03/31 12:32:18 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <cuda_runtime.h>

// Basic operator
__host__ __device__ inline float3 operator+(float3 a, float3 b) {
	return make_float3(a.x + b.x, a.y + b.y, a.z + b.z);
}

__host__ __device__ inline float3 operator+(float3 a, float f) {
	return make_float3(a.x + f, a.y + f, a.z + f);
}


__host__ __device__ inline float3 operator-(float3 a, float3 b) {
	return make_float3(a.x - b.x, a.y - b.y, a.z - b.z);
}

__host__ __device__ inline float3 operator-(float3 a) {
	return make_float3(-a.x, -a.y, -a.z);
}

__host__ __device__ inline float3 operator*(float3 a, float d) {
	return make_float3(a.x * d, a.y * d, a.z * d);
}

__host__ __device__ inline float3 operator*(float d, float3 a) {
	return make_float3(a.x * d, a.y * d, a.z * d);
}

__host__ __device__ inline float3 operator/(float3 a, float d) {
	return make_float3(a.x / d, a.y / d, a.z / d);
}

__host__ __device__ inline float dotProd(float3 a, float3 b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

__host__ __device__ inline float3 vecProd(float3 a, float3 b) {
	return make_float3(a.y * b.z - a.z * b.y,
						a.z * b.x - a.x * b.z,
						a.x * b.y - a.y * b.x);
}

__host__ __device__ inline float3 normalize(float3 a) {
	float len = sqrtf(a.x * a.x + a.y * a.y + a.z * a.z);
	return a / len;
}


// Conversion
__host__ __device__ inline uchar4 toRGBA8(float3 color) {
	color.x = fminf(fmaxf(color.x, 0.0f), 1.0f);
	color.y = fminf(fmaxf(color.y, 0.0f), 1.0f);
	color.z = fminf(fmaxf(color.z, 0.0f), 1.0f);

	// color.x = sqrtf(color.x);
	// color.y = sqrtf(color.y);
	// color.z = sqrtf(color.z);

	return make_uchar4(
		(unsigned char)(color.x * 255.0f),
		(unsigned char)(color.y * 255.0f),
		(unsigned char)(color.z * 255.0f),
		255
	);
}
