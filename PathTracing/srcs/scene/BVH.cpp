/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BVH.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/07 15:14:22 by lde-merc          #+#    #+#             */
/*   Updated: 2026/06/11 18:28:55 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "BVH.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>

BVH::BVH() : _triangles(nullptr) {}

BVH::~BVH() {}

void BVH::build(const std::vector<Triangle>& triangles) {
	_triangles = &triangles;
	_nodes.clear();
	_triangleIndices.clear();
	_rootIndex = -1;

	// Index initialization
	_triangleIndices.resize(triangles.size());
	for (int i = 0; i < triangles.size(); i++)
		_triangleIndices[i] = i;
	
	if (triangles.empty())
		return;
		
	// Build the BVH recursively and store root index
	_rootIndex = buildNode(0, triangles.size());

	std::cout << "\033[32m[BVH]\033[0m \033[33mBuilt with " << _nodes.size() << " nodes, "
			<< triangles.size() << " triangles, root index: " << _rootIndex << "\033[0m\n";
}
AABB BVH::computeAABB(int begin, int end) {
	AABB box;
	for(int i = begin; i < end; ++i) {
		int triIdx = _triangleIndices[i];
		const Triangle& tri = (*_triangles)[triIdx];
		box.expand(tri.v0.pos);
		box.expand(tri.v1.pos);
		box.expand(tri.v2.pos);
	}
	return box;
}


int BVH::buildNode(int begin, int end) {
	// Create a new node
	int nodeIndex = _nodes.size();
	_nodes.emplace_back();

	_nodes[nodeIndex].box = computeAABB(begin, end);	
	int count = end - begin;

	// Create a leaf if we have few enough triangles
	if (count <= LEAF_TRIANGLE_THRESHOLD) {
		_nodes[nodeIndex].start  = begin;
		_nodes[nodeIndex].count  = count;
		_nodes[nodeIndex].left   = -1;
		_nodes[nodeIndex].right  = -1;
		_nodes[nodeIndex].isLeaf = 1;
		return nodeIndex;
	}
	
	// Find the best axis to split at the center
	float3 boxSize = make_float3(
		_nodes[nodeIndex].box.max.x - _nodes[nodeIndex].box.min.x,
		_nodes[nodeIndex].box.max.y - _nodes[nodeIndex].box.min.y,
		_nodes[nodeIndex].box.max.z - _nodes[nodeIndex].box.min.z
	);
	int axis = 0;  // X
	if (boxSize.y > boxSize.x) axis = 1;  // Y
	if (boxSize.z > boxSize.y && boxSize.z > boxSize.x) axis = 2;  // Z
	
	// Compute the center of the bounding box
	float3 center = _nodes[nodeIndex].box.center();
	float splitPos = axis == 0 ? center.x :
					axis == 1 ? center.y : center.z;
					
	// Partition the triangles according to this axis
	int mid = begin;
	for (int i = begin; i < end; ++i) {
		int triIdx = _triangleIndices[i];
		const Triangle& tri = (*_triangles)[triIdx];
		float3 triCenter = make_float3(
			(tri.v0.pos.x + tri.v1.pos.x  + tri.v2.pos.x) / 3.0f,
			(tri.v0.pos.y + tri.v1.pos.y  + tri.v2.pos.y) / 3.0f,
			(tri.v0.pos.z + tri.v1.pos.z  + tri.v2.pos.z) / 3.0f
		);
		float value = (axis == 0) ? triCenter.x :
					(axis == 1) ? triCenter.y : triCenter.z;
		if (value < splitPos) {
			std::swap(_triangleIndices[i], _triangleIndices[mid]);
			mid++;
		}
	}

	// If the partition fails (all on one side), force a split in the middle
	if (mid == begin || mid == end)
		mid = (begin + end) / 2;
	
	// Build child nodes
	_nodes[nodeIndex].isLeaf = 0;
	_nodes[nodeIndex].start  = -1;
	_nodes[nodeIndex].count  = 0;
	
	int left  = buildNode(begin, mid);
	int right = buildNode(mid, end);
	
	_nodes[nodeIndex].left  = left;
	_nodes[nodeIndex].right = right;

	return nodeIndex;
}
