/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SceneObject.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 16:15:10 by lde-merc          #+#    #+#             */
/*   Updated: 2026/06/10 17:27:22 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <cassert>

#include "Exception.hpp"
#include "Mesh.hpp"

class SceneObject {
	public:
		SceneObject();
		~SceneObject();

		void loadFromPath(const std::string);

		std::vector<Triangle> getTriangles() const { return _triangles; }
		std::vector<Material> getMaterials() const { return _materials; }
		std::vector<std::string> getTexturePaths() const { return _texturesPath; }
		size_t getTriangleCount() const { return _triangles.size(); }
		
		void freeGPU();
	private:
		
		// CPU Side
		std::vector<Triangle>		_triangles;
		std::vector<Material>		_materials;
		std::vector<std::string>	_texturesPath;

		// GPU Side
		Triangle*	_d_triangles = nullptr;
		Material*	_d_materials = nullptr;

		// _transform; // Position, rotation, scale de l'objet
};