/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BoidSimulation.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/18 15:35:42 by lde-merc          #+#    #+#             */
/*   Updated: 2026/03/20 19:35:35 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "BoidSimulation.hpp"



BoidSimulation::~BoidSimulation() {
	if (_ssbo) glDeleteBuffers(1, &_ssbo);
}

/************************************************************************
 * Initialization
 * **********************************************************************/

BoidSimulation::BoidSimulation(size_t numBoids, int width, int height) : _numBoids(numBoids),
																_width(width), _height(height) {
	_boids.reserve(_numBoids);
	for (size_t i = 0; i < _numBoids; ++i) {
		_boids.push_back({glm::vec4((static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 2.0f - 1.0f) * 5.0f,
									(static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 2.0f - 1.0f) * 5.0f,
									(static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 2.0f - 1.0f) * 5.0f,
									1.0f),
						  glm::vec4(static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 2.0f - 1.0f,
						 			static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 2.3f - 1.0f,
									static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 1.5f - 1.0f,
									1.0f)
		});
	}

	initSsbo();
	_computeShader = ComputeShader();
	_computeShader.init("shaders/boid.comp");
}

void BoidSimulation::initSsbo() {
	glGenBuffers(1, &_ssbo);
	
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, _ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, _boids.size() * sizeof(Boid), _boids.data(), GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, _ssbo);
}

/************************************************************************
 * Update simulation
 * **********************************************************************/

void BoidSimulation::update(float deltaTime) {
	_computeShader.dispatch(_numBoids, deltaTime);
}

