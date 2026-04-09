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

__device__ bool intersectAABB(const Ray& ray, const AABB& box, float& t0, float& t1) {
	float3 invDir = make_float3(1.0f, 1.0f, 1.0f) / ray.dir;
	float3 tMin = (box.min - ray.origin) * invDir;
	float3 tMax = (box.max - ray.origin) * invDir;

	float3 t1s = fminf(tMin, tMax);
	float3 t2s = fmaxf(tMin, tMax);

	t0 = fmaxf(fmaxf(t1s.x, t1s.y), t1s.z);
	t1 = fminf(fminf(t2s.x, t2s.y), t2s.z);

	return t0 <= t1 && t1 > 0.0001f;
}

__device__ bool intersectTriangle(const Ray& ray, const Triangle& tri, float& t, float& u, float& v) {
	float3 e1 = tri.v1.pos - tri.v0.pos;
	float3 e2 = tri.v2.pos - tri.v0.pos;
	float3 h = vecProd(ray.dir, e2);
	float a = dotProd(e1, h);
	if (fabsf(a) < 1e-6f) return false;

	float f = 1.0f / a;
	float3 s = ray.origin - tri.v0.pos;
	u = f * dotProd(s, h);
	if (u < 0.0f || u > 1.0f) return false;

	float3 q = vecProd(s, e1);
	v = f * dotProd(ray.dir, q);
	if (v < 0.0f || u + v > 1.0f) return false;

	t = f * dotProd(e2, q);
	return t > 0.0001f;
}

__device__ float3 computeNormal(const Triangle& tri) {
	float3 e1 = tri.v1.pos - tri.v0.pos;
	float3 e2 = tri.v2.pos - tri.v0.pos;
	return normalize(vecProd(e1, e2));
}

__device__ bool intersectBVH(const Ray& ray, const BVHNode* nodes, const int* indices,
							const Triangle* triangles, int rootIndex, HitRecord& hit) {
	// Safety check
	if (nodes == nullptr || indices == nullptr || rootIndex < 0) {
		hit.hit = false;
		return false;
	}

	int stack[64];
	int stackPtr = 0;
	stack[stackPtr++] = rootIndex; // Start with root node

	bool hitSomething = false;
	float tMin = 1e20f;

	while (stackPtr > 0) {
		int nodeIdx = stack[--stackPtr];
		const BVHNode& node = nodes[nodeIdx];

		float t0, t1;
		if (!intersectAABB(ray, node.box, t0, t1) || t0 > tMin)
			continue;
		
		if (node.isLeaf) {
			for (int i = 0; i < node.count; ++i) {
				int triIdx = indices[node.start + i];
				float t, u, v;
				if (intersectTriangle(ray, triangles[triIdx], t, u, v) && t < tMin) {
					tMin = t;
					hitSomething = true;
					hit.hit = true;
					hit.t = t;
					hit.normal = computeNormal(triangles[triIdx]);
					hit.posImpact = ray.at(t);
					hit.matIndex = triangles[triIdx].materialIndex;
				}
			}
		} else {
			// Push child nodes onto stack
			if (node.left >= 0)
				stack[stackPtr++] = node.left;
			if (node.right >= 0)
				stack[stackPtr++] = node.right;
		}
	}
	return hitSomething;
}

__global__ void pathTraceKernel(uchar4* fb, int width, int height,
								const Triangle* triangles, int triCount,
								const BVHNode* nodes, const int* bvhIndices, int rootIndex,
								const DirLight* dirLights, int dirLightCount,
								const Material* materials) {
	
	// x and y are the pixel coordinate
	int x = blockIdx.x * blockDim.x + threadIdx.x;
	int y = blockIdx.y * blockDim.y + threadIdx.y;

	if (x >= width || y >= height) return;

	int idx = x + y * width;
	float u = (float)x / width;
	float v = (float)y / height;

	Ray ray = generateRay(d_camera, u, v);
	// Non BVH intersection
	// HitRecord hit = intersectScene(ray, triangles, triCount);
	// if (hit.hit)
	// 	fb[idx] = toRGBA8(hit.normal * 0.5f + 0.5f);
	// else
	// 	fb[idx] = toRGBA8(make_float3(0.0f, 0.0f, 0.6f));

	// BVH intersection
	HitRecord hit;
	hit.hit = false;
	hit.t = 1e30f;
	hit.normal = make_float3(0.0f, 0.0f, 0.0f);

	bool didHit = intersectBVH(ray, nodes, bvhIndices, triangles, rootIndex, hit);
	if (didHit) {
		float3 color = make_float3(0.0f, 0.0f, 0.0f);

		for (int i = 0; i < dirLightCount; i++) {
			const DirLight& light = dirLights[i];

			Ray shadowRay = { hit.posImpact + hit.normal * 0.001f, -light.direction }; // Offset to avoid self-intersection
			HitRecord shadowHit; shadowHit.hit = false;
			intersectBVH(shadowRay, nodes, bvhIndices, triangles, rootIndex, shadowHit);
			if (shadowHit.hit)
				continue; // In shadow, skip this light

			float3 L = light.direction - hit.posImpact;
			float dist2 = dotProd(L, L);
			L = normalize(-light.direction); // Light direction is from light to point

			float NdotL = fmaxf(dotProd(hit.normal, L), 0.0f);

			color = color + materials[hit.matIndex].albedo * light.color * NdotL * light.intensity;
		}

		fb[idx] = toRGBA8(color);
	}
		// fb[idx] = toRGBA8(hit.normal * 0.5f + 0.5f);
	else
		fb[idx] = toRGBA8(make_float3(0.0f, 0.0f, 0.6f));
}


// Lancement kernel
void Compute::update(uchar4* devPtr, const SceneData& scene) {
	dim3 block(16,16);
	dim3 grid((_width + block.x - 1) / block.x,
	          (_height + block.y - 1) / block.y);
	pathTraceKernel<<<grid, block>>>(devPtr, _width, _height,
									scene.triangles, scene.triangleCount,
									scene.bvhNodes, scene.bvhTriangleIndices, scene.bvhRootIndex,
									scene.dirLights, scene.dirLightCount,
									scene.materials);
}

void uploadCamera(const CameraData& cam) {
	GPUCameraData gpuCam;

	gpuCam.origin = make_float3(cam.origin[0], cam.origin[1], cam.origin[2]);
	gpuCam.lowerLeftCorner = make_float3(cam.lowerLeftCorner[0], cam.lowerLeftCorner[1], cam.lowerLeftCorner[2]);
	gpuCam.horizontal = make_float3(cam.horizontal[0], cam.horizontal[1], cam.horizontal[2]);
	gpuCam.vertical = make_float3(cam.vertical[0], cam.vertical[1], cam.vertical[2]);

    cudaMemcpyToSymbol(d_camera, &gpuCam, sizeof(CameraData));
}
