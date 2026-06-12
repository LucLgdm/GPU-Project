/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Scene.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/27 14:37:11 by lde-merc          #+#    #+#             */
/*   Updated: 2026/06/12 11:06:28 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Scene.hpp"

/************************************************************************
 * Helper
 * **********************************************************************/
#define CUDA_CHECK(call)                                                      \
    do {                                                                      \
        cudaError_t err = (call);                                             \
        if (err != cudaSuccess)                                               \
            throw cuda_Error(cudaGetErrorString(err));                        \
    } while (0)
 

/************************************************************************
 * Constructor & Destructor
 * **********************************************************************/
Scene::Scene() {
	std::cout << "\033[32m[Scene]\033[0m \033[33mScene created.\033[0m" << std::endl;
}

Scene::~Scene() {
	freeGPU();
}

void Scene::freeGPU() {
	if (_d_nodes) { cudaFree(_d_nodes); _d_nodes = nullptr; }
	if (_d_triangleIndices) { cudaFree(_d_triangleIndices); _d_triangleIndices = nullptr; }
	if (_d_bvh) { cudaFree(_d_bvh); _d_bvh = nullptr; }
	if (_d_dirLights) { cudaFree(_d_dirLights); _d_dirLights = nullptr; }
	for (auto& texObj : _textureObjects) {
		cudaDestroyTextureObject(texObj);
	}
	for (auto& texArray : _d_textureArrays) {
		cudaFreeArray(texArray);
	}
	for(auto& obj : _objects) {
		obj.freeGPU();
	}
}

/************************************************************************
 * Create Scene
 * **********************************************************************/

void Scene::addObject(std::string filePath, std::string fileName) {
		// Error if object already exists
	if (std::find(_objectsName.begin(), _objectsName.end(), fileName) != _objectsName.end())
		throw std::runtime_error("Object is already loaded\n");
		
	SceneObject obj;
	obj.loadFromPath(filePath);

		// Textures
	int texOffset = static_cast<int>(_textureObjects.size());
		// load
	for (const auto& texPath : obj.getTexturePaths())
		loadTexture(texPath);  // Add in _textureObjects
	_texturesDirty = true;
		// Reorder indexes
	for (auto& mat : obj.getMaterials()) {
		if (mat.textureID != -1)
			mat.textureID += texOffset;
	}
	_materialsDirty = true;
	
	_objects.push_back(std::move(obj));
	_objectsName.push_back(fileName);
	
		// Build BVH
	_bvh.build(getMergedTriangles());
	_bvhDirty = true;

	_trianglesDirty = true;
	_loaded = _objects.size() != 0 ? true : false;
	_SceneUpdated = true;
}

void Scene::removeObject(size_t index) {
	if (index >= _objects.size() || index >= _objectsName.size())
    	return;
	_objects.erase(_objects.begin() + index);
	_objectsName.erase(_objectsName.begin() + index);
	_bvh.build(getMergedTriangles());
	_bvhDirty = true;
	_trianglesDirty = true;
	_loaded = _objects.size() != 0 ? true : false;
	_SceneUpdated = true;
}

std::vector<Triangle> Scene::getMergedTriangles() const {
	std::vector<Triangle> merged;
	int matOffset = 0;

	for (const auto& obj : _objects) {
		for (auto tri : obj.getTriangles()) {
			tri.materialIndex += matOffset;
			merged.push_back(tri);
		}
		matOffset += static_cast<int>(obj.getMaterials().size());
	}
	return merged;
}

std::vector<Material> Scene::getMergedMaterials() const {
	std::vector<Material> merged;

	for (const auto& obj : _objects) {
		const auto& mats = obj.getMaterials();
		merged.insert(merged.end(), mats.begin(), mats.end());
	}
	return merged;
}


void Scene::loadTexture(const std::string& path) {
	int width, height, channels;
	cudaArray* d_textureArray;
	cudaChannelFormatDesc channelDesc = cudaCreateChannelDesc<uchar4>();
	std::cout << "\033[32m[Scene]\033[0m \033[33mLoading texture: " << path << "\033[0m\n";
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 4);
	if (!data) {
		throw inputError("Failed to load texture: " + path);
	}
	
	CUDA_CHECK(cudaMallocArray(&d_textureArray, &channelDesc, width, height));
	CUDA_CHECK(cudaMemcpy2DToArray(d_textureArray, 0, 0, data, width * sizeof(uchar4), width * sizeof(uchar4), height, cudaMemcpyHostToDevice));
	
	stbi_image_free(data);
	
	cudaResourceDesc desc = {};
	desc.res.array.array = d_textureArray;
	desc.resType = cudaResourceTypeArray;

	cudaTextureDesc texDesc = {};
	texDesc.addressMode[0] = cudaAddressModeWrap;
	texDesc.addressMode[1] = cudaAddressModeWrap;
	texDesc.filterMode = cudaFilterModeLinear;
	texDesc.readMode = cudaReadModeNormalizedFloat;
	texDesc.normalizedCoords = 1;
	
	cudaTextureObject_t textureObject;
	CUDA_CHECK(cudaCreateTextureObject(&textureObject, &desc, &texDesc, nullptr));
	
	_textureObjects.push_back(textureObject);
	_d_textureArrays.push_back(d_textureArray);
}

/************************************************************************
 * Upload to GPU
 * **********************************************************************/
void Scene::uploadToGPU() {
	if (_trianglesDirty)
		uploadTriangleToGPU();

	if (_materialsDirty)
		uploadMaterialToGPU();

	if (_texturesDirty)
		uploadTextureToGPU();

	if (_bvhDirty)
		uploadBVHToGPU();

	if (_lightsDirty)
		uploadLightToGPU();
}

void Scene::uploadTriangleToGPU() {
	std::vector<Triangle> triangles = getMergedTriangles();
	size_t triBytes = triangles.size() * sizeof(Triangle);
	if (_d_triangles) {
		CUDA_CHECK(cudaFree(_d_triangles));
		_d_triangles = nullptr;
	}
	CUDA_CHECK(cudaMalloc(&_d_triangles, triBytes));
	CUDA_CHECK(cudaMemcpy(_d_triangles, triangles.data(), triBytes, cudaMemcpyHostToDevice));

	_gpuData.triangles = _d_triangles;
	_gpuData.triangleCount = static_cast<int>(triangles.size());

	std::cout << "\033[32m[Scene]\033[0m \033[33mTriangles upload done ("
			<< triBytes / 1024 << " KB)\033[0m\n";
	_trianglesDirty = false;
}

void Scene::uploadMaterialToGPU() {
	std::vector<Material> materials = getMergedMaterials();
	size_t matBytes = materials.size() * sizeof(Material);
	if (_d_materials) {
		CUDA_CHECK(cudaFree(_d_materials));
		_d_materials = nullptr;
	}
	CUDA_CHECK(cudaMalloc(&_d_materials, matBytes));
	CUDA_CHECK(cudaMemcpy(_d_materials, materials.data(), matBytes, cudaMemcpyHostToDevice));

	_gpuData.materials = _d_materials;
	_gpuData.materialCount = static_cast<int>(materials.size());

	std::cout << "\033[32m[Scene]\033[0m \033[33mMaterials upload done ("
			<< matBytes << " B)\033[0m\n";
	_materialsDirty = false;
}

void Scene::uploadTextureToGPU() {
	if (_d_textureObjects) {
		CUDA_CHECK(cudaFree(_d_textureObjects));
		_d_textureObjects = nullptr;
	}
	
	if (!_textureObjects.empty()) {
		CUDA_CHECK(cudaMalloc(&_d_textureObjects, _textureObjects.size() * sizeof(cudaTextureObject_t)));
		CUDA_CHECK(cudaMemcpy(_d_textureObjects, _textureObjects.data(), _textureObjects.size() * sizeof(cudaTextureObject_t), cudaMemcpyHostToDevice));
		_gpuData.textureObjects = _d_textureObjects;
	} else {
		_gpuData.textureObjects = nullptr;
	}

	_gpuData.textureCount = static_cast<int>(_textureObjects.size());

	std::cout << "\033[32m[Scene]\033[0m \033[33mTextures upload done : "
			<< _gpuData.textureCount << " textures\033[0m\n";
	_texturesDirty = false;
}

void Scene::uploadBVHToGPU() {
	if (_bvh.getNodes().empty()) {
		_gpuData.bvhNodes = nullptr;
		_gpuData.bvhNodeCount = 0;
		_gpuData.bvhTriangleIndices = nullptr;
		return;
	}

	size_t nodeBytes = _bvh.getNodes().size() * sizeof(BVHNode);
	size_t indexBytes = _bvh.getIndices().size() * sizeof(int);

	if (_d_nodes) {
		CUDA_CHECK(cudaFree(_d_nodes));
		_d_nodes = nullptr;
	}
	if (_d_triangleIndices) {
		CUDA_CHECK(cudaFree(_d_triangleIndices));
		_d_triangleIndices = nullptr;
	}
	
	CUDA_CHECK(cudaMalloc(&_d_nodes, nodeBytes));
	CUDA_CHECK(cudaMemcpy(_d_nodes, _bvh.getNodes().data(), nodeBytes, cudaMemcpyHostToDevice));
	CUDA_CHECK(cudaMalloc(&_d_triangleIndices, indexBytes));
	CUDA_CHECK(cudaMemcpy(_d_triangleIndices, _bvh.getIndices().data(), indexBytes, cudaMemcpyHostToDevice));

	_gpuData.bvhNodes = _d_nodes;
	_gpuData.bvhNodeCount = static_cast<int>(_bvh.getNodes().size());
	_gpuData.bvhTriangleIndices = _d_triangleIndices;
	_gpuData.bvhRootIndex = _bvh.getRootIndex();

	std::cout << "\033[32m[Scene]\033[0m \033[33mBVH upload done ("
			<< nodeBytes / 1024 << " KB nodes, "
			<< indexBytes / 1024 << " KB indices, root: "
			<< _gpuData.bvhRootIndex << ")\033[0m\n";
	_bvhDirty = false;
}

void Scene::uploadLightToGPU() {
	size_t dirLightBytes = _dirLights.size() * sizeof(DirLight);

	if (_d_dirLights) {
		CUDA_CHECK(cudaFree(_d_dirLights));
		_d_dirLights = nullptr;
	}

	if (!_dirLights.empty()) {
		CUDA_CHECK(cudaMalloc(&_d_dirLights, dirLightBytes));
		CUDA_CHECK(cudaMemcpy(_d_dirLights, _dirLights.data(), dirLightBytes, cudaMemcpyHostToDevice));
		_gpuData.dirLights = _d_dirLights;
	} else {
		_gpuData.dirLights = nullptr;
	}

	_gpuData.dirLightCount = static_cast<int>(_dirLights.size());

	std::cout << "\033[32m[Scene]\033[0m \033[33mDirectional lights upload done : "
			<< _gpuData.dirLightCount
			<< " lights (" << dirLightBytes / 1024 << " KB)\033[0m\n";
	_lightsDirty = false;
}
