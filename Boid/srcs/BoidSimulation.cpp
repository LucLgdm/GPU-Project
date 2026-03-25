/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BoidSimulation.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/18 15:35:42 by lde-merc          #+#    #+#             */
/*   Updated: 2026/03/25 14:00:12 by lde-merc         ###   ########.fr       */
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
		_boids.push_back({glm::vec4((static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 2.0f - 1.0f) * 2.0f,
									(static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 2.0f - 1.0f) * 2.0f,
									(static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 2.0f - 1.0f) * 2.0f,
									1.0f),
						  glm::vec4((static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 2.0f - 1.0f) * 0.7f,
						 			(static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 2.0f - 1.0f) * 0.7f,
									(static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * 2.0f - 1.0f) * 0.7f,
									1.0f)
		});
	}

	initSsbo();
	initSphereSsbo();
	initCubeSsbo();
	initToreSsbo();
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

void BoidSimulation::initCubeSsbo() {
	glGenBuffers(1, &_ssboCube);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, _ssboCube);
	glBufferData(GL_SHADER_STORAGE_BUFFER, _cube.size() * sizeof(Cube), _cube.data(), GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, _ssboCube);
}

void BoidSimulation::initToreSsbo() {
	glGenBuffers(1, &_ssboTore);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, _ssboTore);
	glBufferData(GL_SHADER_STORAGE_BUFFER, _tore.size() * sizeof(Tore), _tore.data(), GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, _ssboTore);
}

/************************************************************************
 * Update simulation
 * **********************************************************************/

void BoidSimulation::update(float deltaTime) {
	_computeShader.dispatch(_numBoids, deltaTime, _ssboSphere, _sphere.size(), _cube.size(), _tore.size());
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

void BoidSimulation::updateSpherePos(glm::vec4 positionRadius, int i) {
	_sphere[i].positionRadius = positionRadius;
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, _ssboSphere);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, i * sizeof(Sphere), sizeof(Sphere), &_sphere[i]);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}


void BoidSimulation::addCube(glm::vec4 min, glm::vec4 max) {
	_cube.push_back({min, max});
	updateCubeSsbo();
}

void BoidSimulation::updateCubeSsbo() {
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, _ssboCube);
	glBufferData(GL_SHADER_STORAGE_BUFFER, _cube.size() * sizeof(Cube), _cube.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void BoidSimulation::updateCubePos(glm::vec4 min, glm::vec4 max, int i) {
	_cube[i].max = max;
	_cube[i].min = min;
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, _ssboCube);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, i * sizeof(Cube), sizeof(Cube), &_cube[i]);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);	
}


void BoidSimulation::addTore(glm::vec4 position, glm::vec4 radii) {
	_tore.push_back({position, radii});
	updateToreSsbo();
}

void BoidSimulation::updateToreSsbo() {
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, _ssboTore);
	glBufferData(GL_SHADER_STORAGE_BUFFER, _tore.size() * sizeof(Tore), _tore.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void BoidSimulation::updateTorePos(glm::vec4 pos, glm::vec4 rad, int i) {
	_tore[i].position = pos;
	_tore[i].radii = rad;	
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, _ssboTore);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, i * sizeof(Tore), sizeof(Tore), &_tore[i]);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);	
}
