/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Ray.cuh	                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/31 13:04:59 by lde-merc          #+#    #+#             */
/*   Updated: 2026/03/31 12:32:18 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Scene.hpp"

struct Ray {
    float3 origin;
    float3 dir;

    __device__ float3 at(float t) const {
        return origin + t * dir;
    }
};

struct HitRecord {
	float t;
	float3 posImpact;
	float3 normal;
	int matIndex;
	bool hit;
};

__device__ float intersect(Ray r, Triangle tri) {
	float3 e1 = tri.v1.pos - tri.v0.pos, e2 = tri.v2.pos - tri.v0.pos;
	float3 h = vecProd(r.dir, e2);
	float a = dotProd(e1, h);
	if (fabsf(a) < 1e-6f) return -1;
	
	float f = 1 / a;
	float3 s = r.origin - tri.v0.pos;
	float u = f * dotProd(s, h);
	if (u > 1.0f || u < 0.0f) return -1;

	float3 q = vecProd(s, e1);
	float v = f * dotProd(r.dir, q);
	if (v > 1.0f || v < 0.0f || u + v > 1.0f) return -1;

	float t = f * dotProd(e2, q);
	if (t < 0.0001) return -1;
	return t;
}

__device__ HitRecord intersectScene(const Ray& ray, const Triangle* triangles, int triCount) {
	HitRecord hit;
	hit.t = 1e30f;
	hit.hit = false;

	for (int i = 0; i < triCount; i++) {
		float dist = intersect(ray, triangles[i]);
		if (dist > 0 && dist < hit.t) {
			hit.t = dist;  
			hit.posImpact = ray.at(hit.t);
			hit.hit = true;

			// normale géométrique
			float3 e1 = triangles[i].v1.pos - triangles[i].v0.pos;
			float3 e2 = triangles[i].v2.pos - triangles[i].v0.pos;
			hit.normal = normalize(vecProd(e1, e2));

			hit.matIndex = triangles[i].materialIndex;
		}
	}
	return hit;
}

