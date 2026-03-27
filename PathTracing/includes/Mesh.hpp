/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mesh.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/27 14:28:17 by lde-merc          #+#    #+#             */
/*   Updated: 2026/03/27 15:56:47 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <cuda_runtime.h>

struct Vertex {
	float3 pos;
	float3 normal;
	float2 uv;
};

struct Triangle {
	Vertex v0, v1, v2;
	int    materialIndex;
};

struct Material {
    float3 albedo;       // Kd — couleur diffuse
    float3 emission;     // Ke — lumière émise (0 si pas une source)
    float  roughness;    // 1 - Ns normalisé
    float  metallic;     // 0 ou 1 selon le workflow
    float  ior;          // Ni — indice de réfraction (verre = 1.5)
    float  opacity;      // d — 1.0 = opaque
};

