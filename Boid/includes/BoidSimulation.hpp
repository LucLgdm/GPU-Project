/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BoidSimulation.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/18 15:23:01 by lde-merc          #+#    #+#             */
/*   Updated: 2026/03/24 14:54:54 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"

#include <vector>
#include <cstdlib>
#include <iostream>
#include "ComputeShader.hpp"

// On utilise des vec4 et non des vec3 car le glsl a un padding de 4
// Avec un vec3, le C++ voit 12 bytes + 12 bytes = 24 bytes
// et le glsl 16 bytes + 16 bytes = 32 bytes (padding) d'ou le vec4
struct Boid {
	glm::vec4 position;
	glm::vec4 velocity;
};

struct Sphere {
	glm::vec4 positionRadius;
};

// A cube is an AABB (Axis-Aligned Bounding-Box)
struct Cube {
	glm::vec4 min;
	glm::vec4 max;
};

struct Tore {
	glm::vec4 position;
	glm::vec2 radii;
	glm::vec2 padding;
};

class BoidSimulation {
	public:
		BoidSimulation(size_t numBoids, int width, int height);
		~BoidSimulation();

		void update(float deltaTime);
		
		const std::vector<Boid>& getBoids() const { return _boids; };
		const GLuint& getSsbo() const { return _ssbo; };
		ComputeShader& getCS()	{return _computeShader; };
		const std::vector<Sphere>& getSphere() const {return _sphere; };
		const std::vector<Cube>& getCube() const {return _cube; };
		const std::vector<Tore>& getTore() const {return _tore; };
		
		void updateBoundSize(float size) {_computeShader.updateBoundSize(size); };

		void addSphere(glm::vec3, float);
		void updateSphereSsbo();

		void addCube(glm::vec4, glm::vec4);
		void updateCubeSsbo();

		void addTore(glm::vec4, glm::vec2);
		void updateToreSsbo();
		
	private:
		int _width;
		int _height;
		
		std::vector<Boid> _boids;
		size_t _numBoids;

		GLuint _ssbo; // Shader Storage Buffer Object
		ComputeShader _computeShader;

		std::vector<Sphere> _sphere;
		GLuint _ssboSphere;

		std::vector<Cube> _cube;
		GLuint _ssboCube;

		std::vector<Tore> _tore;
		GLuint _ssboTore;
		
		void initSsbo();
		void initSphereSsbo();
		void initCubeSsbo();
		void initToreSsbo();
};
