/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BoidSimulation.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/18 15:35:42 by lde-merc          #+#    #+#             */
/*   Updated: 2026/03/23 15:51:04 by lde-merc         ###   ########.fr       */
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
		_boids.push_back({glm::vec4((static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 2.0f - 1.0f) * 15.0f,
									(static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 2.0f - 1.0f) * 15.0f,
									(static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 2.0f - 1.0f) * 15.0f,
									1.0f),
						  glm::vec4((static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 2.0f - 1.0f) * 15.0f,
						 			(static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 2.0f - 1.0f) * 15.0f,
									(static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 2.0f - 1.0f) * 15.0f,
									1.0f)
		});
	}

	initSsbo();
	initSphereSsbo();
	addSphere(glm::vec3(0.0f, 0.0f, 0.0f), 2.0f);
	_computeShader = ComputeShader();
	_computeShader.init("shaders/boid.comp");
}

void BoidSimulation::initSsbo() {
	glGenBuffers(1, &_ssbo);
	
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, _ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, _boids.size() * sizeof(Boid), _boids.data(), GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, _ssbo);
}

void BoidSimulation::initSphereSsbo() {
	glGenBuffers(1, &_ssboSphere);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, _ssboSphere);
	glBufferData(GL_SHADER_STORAGE_BUFFER, _sphere.size() * sizeof(Sphere), _sphere.data(), GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, _ssboSphere);
}

/************************************************************************
 * Update simulation
 * **********************************************************************/

void BoidSimulation::update(float deltaTime) {
	_computeShader.dispatch(_numBoids, deltaTime, _ssboSphere, _sphere.size());
}

void BoidSimulation::addSphere(glm::vec3 position, float radius) {
	_sphere.push_back({glm::vec4(position, radius)});
	updateSphereSsbo();
}

void BoidSimulation::updateSphereSsbo() {
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, _ssboSphere);
    glBufferData(GL_SHADER_STORAGE_BUFFER, _sphere.size() * sizeof(Sphere), _sphere.data(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}