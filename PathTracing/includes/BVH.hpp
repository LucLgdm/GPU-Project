/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BVH.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/07 14:03:52 by lde-merc          #+#    #+#             */
/*   Updated: 2026/04/07 17:40:53 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <cuda_runtime.h>
#include <vector>
#include <algorithm>
#include "Mesh.hpp"

struct AABB {
	float3 min;
	float pad1;
	float3 max;
	float pad2;

	AABB() : min{1e9f, 1e9f, 1e9f}, max{-1e9f, -1e9f, -1e9f} {}
	AABB(float3 _min, float3 _max) : min(_min), max(_max) {}

	void expand(const float3& point) {
		min.x = std::min(min.x, point.x);
		min.y = std::min(min.y, point.y);
		min.z = std::min(min.z, point.z);
		max.x = std::max(max.x, point.x);
		max.y = std::max(max.y, point.y);
		max.z = std::max(max.z, point.z);
	}

	void expand(const AABB& other) {
		expand(other.min);
		expand(other.max);
	}

	float3 center() const {
		return make_float3(
			(min.x + max.x) * 0.5f,
			(min.y + max.y) * 0.5f,
			(min.z + max.z) * 0.5f
		);
	}

	float surfaceArea() const {
		float3 d = make_float3(max.x - min.x, max.y - min.y, max.z - min.z);
		return 2.0f * (d.x * d.y + d.y * d.z + d.z * d.x);
	}
};

struct BVHNode {
	AABB box;

	int left;   // index dans le tableau (ou -1)
	int right;  // index dans le tableau (ou -1)

	int start;  // index dans triangles
	int count;  // nombre de triangles

	int isLeaf;
};

class BVH {
	public:
		BVH();
		~BVH();

		void build(const std::vector<Triangle>& triangles);
		
		const std::vector<BVHNode>& getNodes() const { return _nodes; }
		const std::vector<int>& getIndices() const { return _triangleIndices; }
		int getRootIndex() const { return _rootIndex; }

	private:
		std::vector<BVHNode> _nodes;
		std::vector<int> _triangleIndices;  // indices réordonnés des triangles
		const std::vector<Triangle>* _triangles;
		int _rootIndex = -1;

		int buildNode(int begin, int end);
		AABB computeAABB(int begin, int end);
		
		static const int LEAF_TRIANGLE_THRESHOLD = 4;
};
