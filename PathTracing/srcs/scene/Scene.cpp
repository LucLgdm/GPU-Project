/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Scene.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/27 14:37:11 by lde-merc          #+#    #+#             */
/*   Updated: 2026/06/09 16:56:48 by lde-merc         ###   ########.fr       */
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
 * Initialization
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
		loadTexture(texPath);  // ajoute dans _textureObjects
		// Reorder indexes
	for (auto& mat : obj.getMaterials()) {
		if (mat.textureID != -1)
			mat.textureID += texOffset;
	}
	
	_objects.push_back(std::move(obj));
	_objectsName.push_back(fileName);
	
		// Build BVH
	_bvh.build(getMergedTriangles());
	
		// Lights (for testing, should be loaded from file)
	// _dirLights.push_back({ make_float3(1.0f, 0.3f, 0.0f), make_float3(5.0f, 5.0f, 5.0f), 0.2f });
	// _dirLights.back().direction = normalize(-_dirLights.back().direction);

	// uploadToGPU();
	_loaded = true;
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

// std::vector<Material> Scene::getMergedMaterials() const {
// 	std::vector<Material> merged;

// 	for (const auto& obj : _objects) {
// 		const auto& mats = obj.getMaterials();
// 		merged.insert(merged.end(), mats.begin(), mats.end());
// 	}
// 	return merged;
// }


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

 /*


void Scene::uploadToGPU() {
		// Fusionner depuis les objets
    std::vector<Triangle> triangles = getMergedTriangles();
    std::vector<Material> materials = getMergedMaterials();

    	// --- TRIANGLES ---
    size_t triBytes = triangles.size() * sizeof(Triangle);
    Triangle* d_triangles = nullptr;
    CUDA_CHECK(cudaMalloc(&d_triangles, triBytes));
    CUDA_CHECK(cudaMemcpy(d_triangles, triangles.data(), triBytes, cudaMemcpyHostToDevice));

    	// --- MATERIALS ---
    size_t matBytes = materials.size() * sizeof(Material);
    Material* d_materials = nullptr;
    CUDA_CHECK(cudaMalloc(&d_materials, matBytes));
    CUDA_CHECK(cudaMemcpy(d_materials, materials.data(), matBytes, cudaMemcpyHostToDevice));

    	// --- TEXTURES ---
    CUDA_CHECK(cudaMalloc(&_d_textureObjects, _textureObjects.size() * sizeof(cudaTextureObject_t)));
    CUDA_CHECK(cudaMemcpy(_d_textureObjects, _textureObjects.data(),
               _textureObjects.size() * sizeof(cudaTextureObject_t), cudaMemcpyHostToDevice));

    	// --- GPU DATA ---
    _gpuData.triangles      = d_triangles;
    _gpuData.triangleCount  = static_cast<int>(triangles.size());
    _gpuData.materials      = d_materials;
    _gpuData.materialCount  = static_cast<int>(materials.size());
    _gpuData.textureObjects = _d_textureObjects;
    _gpuData.textureCount   = static_cast<int>(_textureObjects.size());
	
	std::cout << "\033[32m[Scene]\033[0m \033[33mGPU upload done ("
			<< triBytes / 1024 << " KB triangles, "
			<< matBytes        << " B materials)\033[0m\n";
	
		// --- BVH ---
	size_t nodeBytes = _bvh.getNodes().size() * sizeof(BVHNode);
	size_t indexBytes = _bvh.getIndices().size() * sizeof(int);

	if (_bvh.getNodes().empty()) {
		_gpuData.bvhNodes = nullptr;
		_gpuData.bvhNodeCount = 0;
		_gpuData.bvhTriangleIndices = nullptr;
		return;
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
			<< indexBytes / 1024 << " KB indices, root: " << _gpuData.bvhRootIndex << ")\033[0m\n";

	// --- DIRECTIONAL LIGHTS ---
	size_t dirLightBytes = _dirLights.size() * sizeof(DirLight);
	CUDA_CHECK(cudaMalloc(&_d_dirLights, dirLightBytes));
	CUDA_CHECK(cudaMemcpy(_d_dirLights, _dirLights.data(), dirLightBytes, cudaMemcpyHostToDevice));

	_gpuData.dirLights = _d_dirLights;
	_gpuData.dirLightCount = static_cast<int>(_dirLights.size());
	
	std::cout << "\033[32m[Scene]\033[0m \033[33mDirectional lights upload done : "
			<< _gpuData.dirLightCount << " lights ("
			<< dirLightBytes / 1024 << " KB)\033[0m\n";
}

*/