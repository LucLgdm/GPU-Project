/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SceneObject.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 16:21:16 by lde-merc          #+#    #+#             */
/*   Updated: 2026/06/09 16:52:14 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include "SceneObject.hpp"


// static float3 toFloat3(const tinyobj::real_t* v, int offset = 0) {
//     return make_float3(v[offset], v[offset + 1], v[offset + 2]);
// }



SceneObject::SceneObject() { }

SceneObject::~SceneObject() { } // Handled by Scene destructor

void SceneObject::freeGPU() {
	if (_d_triangles) { cudaFree(_d_triangles); _d_triangles = nullptr; }
	if (_d_materials) { cudaFree(_d_materials); _d_materials = nullptr; }
}

void SceneObject::loadFromPath(const std::string filePath) {
	tinyobj::ObjReaderConfig config;
	config.mtl_search_path = filePath.substr(0, filePath.find_last_of("/\\") + 1);
	config.triangulate = true;  // force the triangulation

	tinyobj::ObjReader reader;
	if (!reader.ParseFromFile(filePath, config))
		throw tinyObjLoader_Error(reader.Error());
		
	if (!reader.Warning().empty()) {
		std::cerr << "[Scene] Warning: " << reader.Warning() << "\n";
	}

	const auto& attrib    = reader.GetAttrib();
	const auto& shapes    = reader.GetShapes();
	const auto& materials = reader.GetMaterials();

		// Material
	_materials.reserve(materials.size() + 1);
	for (const auto& m : materials) {
		Material mat;
		mat.albedo = make_float3(m.diffuse[0], m.diffuse[1], m.diffuse[2]);
		mat.emission = make_float3(m.emission[0], m.emission[1], m.emission[2]);
		mat.roughness = sqrtf(2.0f / (m.shininess + 2.0f)); // Ns [0; 1000]
		mat.metallic = 0.0f;
		mat.ior = m.ior > 0.0f ? m.ior : 1.0f;
		mat.opacity = m.dissolve;
		if (!m.diffuse_texname.empty()) {
			std::string texPath = config.mtl_search_path + m.diffuse_texname;
			_texturesPath.push_back(texPath);
			mat.textureID = static_cast<int>(_texturesPath.size() - 1);
		} else {
			mat.textureID = -1;
		}
		_materials.push_back(mat);
	}

	// Default material
	if (_materials.empty()) {
		Material def;
		def.albedo    = make_float3(0.8f, 0.8f, 0.8f);
		def.emission  = make_float3(0.0f, 0.0f, 0.0f);
		def.roughness = 1.0f;
		def.metallic  = 0.0f;
		def.ior       = 1.0f;
		def.opacity   = 1.0f;
		def.textureID = -1;
		_materials.push_back(def);
	}

		// Triangle
	for(const auto& shape : shapes) {
		const auto& mesh = shape.mesh;
		size_t indexOffset = 0;

		for (size_t f = 0; f < mesh.num_face_vertices.size(); ++f) {
			assert(mesh.num_face_vertices[f] == 3);
			
			Triangle tri;
			tri.materialIndex = mesh.material_ids[f] >= 0 ? mesh.material_ids[f] : 0;
			for (int v = 0; v < 3; ++v) {
				tinyobj::index_t idx = mesh.indices[indexOffset + v];

				// Position
				float3 pos = make_float3(
					attrib.vertices[3 * idx.vertex_index],
					attrib.vertices[3 * idx.vertex_index + 1],
					attrib.vertices[3 * idx.vertex_index + 2]
				);

				// Normal 
				float3 normal = make_float3(0.0f, 0.0f, 0.0f);
				if (idx.normal_index >= 0 ) {
					normal = make_float3(
						attrib.normals[3 * idx.normal_index],
						attrib.normals[3 * idx.normal_index + 1],
						attrib.normals[3 * idx.normal_index + 2]
					);
				}

				// UV
				float2 uv = make_float2(0.0f, 0.0f);
				if (idx.texcoord_index >= 0) {
					uv = make_float2(
						attrib.texcoords[2 * idx.texcoord_index],
						attrib.texcoords[2 * idx.texcoord_index + 1]
					);
				}

				Vertex vert;
				vert.pos	= pos;
				vert.normal	= normal;
				vert.uv		= uv;

				if (v == 0) tri.v0 = vert;
				if (v == 1) tri.v1 = vert;
				if (v == 2) tri.v2 = vert;
			}
			// Compute the normal if not present
			bool hasNormal =
				(tri.v0.normal.x != 0 || tri.v0.normal.y != 0 || tri.v0.normal.z != 0) ||
				(tri.v1.normal.x != 0 || tri.v1.normal.y != 0 || tri.v1.normal.z != 0) ||
				(tri.v2.normal.x != 0 || tri.v2.normal.y != 0 || tri.v2.normal.z != 0);

			if (!hasNormal) {
				float3 e1 = make_float3(
					tri.v1.pos.x - tri.v0.pos.x,
					tri.v1.pos.y - tri.v0.pos.y,
					tri.v1.pos.z - tri.v0.pos.z
				);
				
				float3 e2 = make_float3(
					tri.v2.pos.x - tri.v0.pos.x,
					tri.v2.pos.y - tri.v0.pos.y,
					tri.v2.pos.z - tri.v0.pos.z
				);
				
				// dot product
				float3 n = make_float3(
					e1.y * e2.z - e1.z * e2.y,
					e1.z * e2.x - e1.x * e2.z,
					e1.x * e2.y - e1.y * e2.x
				);
				
				// normalize
				float len = sqrtf(n.x * n.x + n.y * n.y + n.z * n.z);
				if (len > 1e-6f) n = make_float3(n.x / len, n.y / len, n.z / len);
				tri.v0.normal = tri.v1.normal = tri.v2.normal = n;
			}
			_triangles.push_back(tri);
			indexOffset += 3;
		}
	}
	std::cout << "\033[32m[SceneObject]\033[0m \033[33mLoaded: " << _triangles.size() << " triangles, "
			<< _materials.size() << " materials\033[0m\n";
}
