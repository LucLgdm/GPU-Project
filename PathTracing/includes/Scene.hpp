/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Scene.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/27 14:34:17 by lde-merc          #+#    #+#             */
/*   Updated: 2026/06/12 15:51:50 by lde-merc         ###   ########.fr       */
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

	SpotLight*	spotLights;
	int 		spotLightCount;
};

// This class load a .obj on the CPU and send it to the GPU
class Scene {
	public:
		Scene();
		~Scene();
		
		// ---Object---
		void addObject(std::string, std::string);
		void loadTexture(const std::string&);

		void removeObject(size_t);

		std::vector<Triangle> getMergedTriangles() const;
		std::vector<Material> getMergedMaterials() const;

		// ---Light---
		void addDirLight(DirLight light);
		void removeDirLight(size_t);
		void addSpotLight(SpotLight light);
		void removeSpotLight(size_t);
		
		// ---Getter && Setter---
		const BVH& getBVH() const { return _bvh; }
		SceneData getGpuData() const { return _gpuData; } // Struct ready to be sent to the kernel
		std::vector<SceneObject>& getObjects() { return _objects; }
		const std::vector<std::string>& getObjName() const { return _objectsName; }
		const std::vector<SceneObject>& getObjects() const { return _objects; }
		const std::vector<DirLight>& getDirLights() const { return _dirLights; }
		DirLight& getDirLight(size_t i) {return _dirLights[i]; }
		const std::vector<SpotLight>& getSpotLights() const { return _spotLights; }
		SpotLight& getSpotLight(size_t i) {return _spotLights[i]; }
	
		bool isLoaded() const { return _loaded; }
		bool isUpdated() const { return _SceneUpdated; }
		void setUpdated(bool up) { _SceneUpdated = up; }
		void setDirLightDirty(bool up) { _lightsDirDirty = up; }
		void setSpotLightDirty(bool up) { _lightsSpotDirty = up; }

		// ---From CPU to GPU---
		void uploadTriangleToGPU();
		void uploadMaterialToGPU();
		void uploadTextureToGPU();
		void uploadBVHToGPU();
		void uploadLightToGPU();
		void uploadToGPU();
		
	private:
		// ---CPU side---
		std::vector<SceneObject> _objects;
		std::vector<std::string> _objectsName;
		std::vector<DirLight> _dirLights;
		std::vector<SpotLight> _spotLights;
		BVH _bvh;
	
		std::vector<cudaTextureObject_t> _textureObjects;
		std::vector<cudaArray*> _d_textureArrays;
		
		// ---GPU side---
		Triangle*	_d_triangles = nullptr;
		Material*	_d_materials = nullptr;
		BVH*		_d_bvh = nullptr;
		BVHNode*	_d_nodes = nullptr;
		int*		_d_triangleIndices = nullptr;
		DirLight*	_d_dirLights = nullptr;
		SpotLight*	_d_spotLights = nullptr;
		cudaTextureObject_t* _d_textureObjects = nullptr;
		
		bool _trianglesDirty = false;
		bool _materialsDirty = false;
		bool _texturesDirty = false;
		bool _bvhDirty = false;
		bool _lightsDirDirty = false;
		bool _lightsSpotDirty = false;
		
		// ---Data struct pour le kernel---
		bool		_SceneUpdated = false;
		bool		_loaded  = false;
		SceneData	_gpuData = {};

		void	freeGPU();
};