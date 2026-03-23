/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BoidSimulation.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/18 15:23:01 by lde-merc          #+#    #+#             */
/*   Updated: 2026/03/23 12:23:42 by lde-merc         ###   ########.fr       */
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


class BoidSimulation {
	public:
		BoidSimulation(size_t numBoids, int width, int height);
		~BoidSimulation();

		void update(float deltaTime);
		
		const std::vector<Boid>& getBoids() const { return _boids; };
		const GLuint& getSsbo() const { return _ssbo; };
		ComputeShader& getCS()	{return _computeShader; };

		void updateBoundSize(float size) {_computeShader.updateBoundSize(size); };

	private:
		int _width;
		int _height;
		
		std::vector<Boid> _boids;
		size_t _numBoids;

		GLuint _ssbo; // Shader Storage Buffer Object
		ComputeShader _computeShader;
		void initSsbo();
};
