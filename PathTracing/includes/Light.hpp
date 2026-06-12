/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Light.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/08 15:59:19 by lde-merc          #+#    #+#             */
/*   Updated: 2026/06/11 17:55:34 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <cuda_runtime.h>

struct DirLight {
	float3 direction;
	float3 color;
	float intensity;

	DirLight();
	DirLight(float3 dir, float3 col, float i)
		: direction(dir), color(col), intensity(i) {}

	void addColor(float r, float g, float b) {
		color.x = r; color.y = g; color.z = b;
	}

	void addDirection(float x, float y, float z) {
		direction.x = x; direction.y = y; direction.z = z;
	}

	void addIntensity(float i) {
		intensity = i;
	}
};