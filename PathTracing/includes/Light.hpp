/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Light.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/08 15:59:19 by lde-merc          #+#    #+#             */
/*   Updated: 2026/06/12 14:13:12 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <cuda_runtime.h>

struct DirLight {
	float3	direction;
	float3	color;
	float	intensity;
	int		index;

	DirLight();
	DirLight(float3 dir, float3 col, float i)
		: direction(dir), color(col), intensity(i) {}
	
	void getColor(float out[3]) const {
		out[0] = color.x;
		out[1] = color.y;
		out[2] = color.z;
	}

	void getDirection(float out[3]) const {
		out[0] = direction.x;
		out[1] = direction.y;
		out[2] = direction.z;
	}

	void setColor(float in[3]) {
		color.x = in[0];
		color.y = in[1];
		color.z = in[2];
	}

	void setIntensity(float intens) {
		intensity = intens;
	}

	void setDirection(float yaw, float pitch) {
		float yawRad = yaw * M_PI / 180.0f;
		float pitchRad = pitch * M_PI / 180.0f;

		direction.x = cosf(pitchRad) * cosf(yawRad);
		direction.y = sinf(pitchRad);
		direction.z = cosf(pitchRad) * sinf(yawRad);
	}
};