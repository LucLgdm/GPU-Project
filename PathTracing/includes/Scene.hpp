/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Scene.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/27 14:34:17 by lde-merc          #+#    #+#             */
/*   Updated: 2026/06/10 13:31:09 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <stdexcept>
#include <cmath>

#include "Exception.hpp"
#include "BVH.hpp"
#include "SceneObject.hpp"
#include "Light.hpp"
#include "MathUtils.cuh"

// A struct to pass to the kernel
struct SceneData {
	Triangle*	triangles;
	int			triangleCount;
	
	Material*	materials;
	int			materialCount;

	cudaTextureObject_t*	textureObjects;
	int						textureCount;

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
		
		void addObject(std::string, std::string);
		void loadTexture(const std::string&);

		std::vector<Triangle> getMergedTriangles() const;
		std::vector<Material> getMergedMaterials() const;

		const BVH& getBVH() const { return _bvh; }


		// Struct ready to be sent to the kernel
		SceneData getGpuData() const { return _gpuData; }
	
		bool isLoaded() const { return _loaded; }
		
	private:
		// CPU side
		std::vector<SceneObject> _objects;
		std::vector<std::string> _objectsName;
		std::vector<DirLight> _dirLights;
		BVH _bvh;
	
		std::vector<cudaTextureObject_t> _textureObjects;
		std::vector<cudaArray*> _d_textureArrays;
		
		// GPU side
		BVH*		_d_bvh = nullptr;
		BVHNode*	_d_nodes = nullptr;
		int*		_d_triangleIndices = nullptr;
		
		DirLight*	_d_dirLights = nullptr;
		
		cudaTextureObject_t* _d_textureObjects;
		
		// Data struct pour le kernel
		SceneData	_gpuData = {};
		bool		_loaded  = false;

		// Helper
		void	uploadToGPU();
		void	freeGPU();
};