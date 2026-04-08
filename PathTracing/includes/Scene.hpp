/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Scene.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/27 14:34:17 by lde-merc          #+#    #+#             */
/*   Updated: 2026/04/08 16:03:56 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <stdexcept>
#include <cmath>
#include <cassert>

#include "Exception.hpp"
#include "BVH.hpp"
#include "Light.hpp"

// A struct to pass to the kernel
struct SceneData {
	Triangle*	triangles;
	int			triangleCount;
	
	Material*	materials;
	int			materialCount;

	BVHNode*	bvhNodes;
	int			bvhNodeCount;
	int*		bvhTriangleIndices;
	int			bvhRootIndex;

	DirLight*	dirLights;
	int			dirLightCount;
};

// This class load a .obj on the CPU and send it to the GPU
class Scene {
	public:
		Scene();
		~Scene();
		
		void load(const std::string&);

		const std::vector<Triangle>& getTriangles() const { return _triangles; }
		const std::vector<Material>& getMaterials()  const { return _materials; }
		const BVH& getBVH() const { return _bvh; }
	
		// Struct prête à être passée au kernel CUDA
		SceneData getGpuData() const { return _gpuData; }
	
		bool isLoaded() const { return _loaded; }
		
	private:
		// CPU side
		std::vector<Triangle> _triangles;
		std::vector<Material> _materials;
		std::vector<DirLight> _dirLights;
		BVH _bvh;
	
		// GPU side
		Triangle*	_d_triangles = nullptr;
		Material*	_d_materials = nullptr;
		
		BVH*		_d_bvh = nullptr;
		BVHNode*	_d_nodes = nullptr;
		int*		_d_triangleIndices = nullptr;
		
		DirLight*	_d_dirLights = nullptr;
		
		SceneData	_gpuData = {};
		bool		_loaded  = false;

		void	uploadToGPU();
		void	freeGPU();
};