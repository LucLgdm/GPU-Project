/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Renderer.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/18 15:35:33 by lde-merc          #+#    #+#             */
/*   Updated: 2026/03/25 13:56:55 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Renderer.hpp"

Renderer::Renderer(): _width(0), _height(0), _shaderProgram(0),
						_VAO(0), _VBO(0), _numBoids(0) { }

Renderer::~Renderer() { 
	if (_VAO) glDeleteVertexArrays(1, &_VAO);
	if (_VBO) glDeleteBuffers(1, &_VBO);
	if (_shaderProgram) glDeleteProgram(_shaderProgram);
}

/************************************************************************
 * Initialization
 * **********************************************************************/

void Renderer::init(int width, int height, int numBoid) {
	_width = width;
	_height = height;
	_numBoids = numBoid;
	initShaders();
	createBuffers();
	initBox(); initSphere(); initCube(); initTorus();
}

static std::string readFile(const char* path) {
	std::ifstream file(path);
	if (!file.is_open())
		throw inputError(std::string("Cannot open shader file: ") + path);

	std::stringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
}

void Renderer::initShaders() {
	const char* vPath = "shaders/vertex.glsl";
	const char* fPath = "shaders/fragment.glsl";
	const char* bvPath = "shaders/box_vertex.glsl";
	const char* bfPath = "shaders/box_fragment.glsl";
	
	std::string vCode = readFile(vPath);
	std::string fCode = readFile(fPath);
	
	std::string bvCode = readFile(bvPath);
	std::string bfCode = readFile(bfPath);
	
	const char* vSrc = vCode.c_str();
	const char* fSrc = fCode.c_str();
	
	const char* bvSrc = bvCode.c_str();
	const char* bfSrc = bfCode.c_str();

	GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vShader, 1, &vSrc, nullptr);
	glCompileShader(vShader);

	GLint success;
	glGetShaderiv(vShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		char log[512];
		glGetShaderInfoLog(vShader, 512, nullptr, log);
		throw openGlError(log);
	}

	GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fShader, 1, &fSrc, nullptr);
	glCompileShader(fShader);
	
	glGetShaderiv(fShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		char log[512];
		glGetShaderInfoLog(fShader, 512, nullptr, log);
		throw openGlError(log);
	}
	
	_shaderProgram = glCreateProgram();

	GLuint bvShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(bvShader, 1, &bvSrc, nullptr);
	glCompileShader(bvShader);

	glGetShaderiv(bvShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		char log[512];
		glGetShaderInfoLog(bvShader, 512, nullptr, log);
		throw openGlError(log);
	}

	GLuint bfShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(bfShader, 1, &bfSrc, nullptr);
	glCompileShader(bfShader);

	glGetShaderiv(bfShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		char log[512];
		glGetShaderInfoLog(bfShader, 512, nullptr, log);
		throw openGlError(log);
	}
	
	_boxShaderProgram = glCreateProgram();

	glAttachShader(_shaderProgram, vShader);
	glAttachShader(_shaderProgram, fShader);
	glLinkProgram(_shaderProgram);

	glGetShaderiv(bfShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		char log[512];
		glGetShaderInfoLog(bfShader, 512, nullptr, log);
		throw openGlError(log);
	}

	glAttachShader(_boxShaderProgram, bvShader);
	glAttachShader(_boxShaderProgram, bfShader);
	glLinkProgram(_boxShaderProgram);

	glGetShaderiv(bfShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		char log[512];
		glGetShaderInfoLog(bfShader, 512, nullptr, log);
		 throw openGlError(log);
	}
	
	glDeleteShader(vShader);
	glDeleteShader(fShader);
	glDeleteShader(bvShader);
	glDeleteShader(bfShader);
}

void Renderer::createBuffers() {
	const int segments = 20;
	const float radius = 0.75 / 15.0f;
	const float height = 1.0 / 15.0f;
	const float baseY = -1.0 / 15.0f;

	const float PI = 3.1415926535f;

	std::vector<float> vertices;

	// ===== Faces latérales =====
	for (int i = 0; i < segments; ++i) {
		float angle1 = 2.f * PI * i / segments;
		float angle2 = 2.f * PI * (i + 1) / segments;

		float x1 = radius * cos(angle1);
		float z1 = radius * sin(angle1);

		float x2 = radius * cos(angle2);
		float z2 = radius * sin(angle2);

		// Apex
		vertices.push_back(0.f);
		vertices.push_back(height);
		vertices.push_back(0.f);

		// Base point 1
		vertices.push_back(x1);
		vertices.push_back(baseY);
		vertices.push_back(z1);

		// Base point 2
		vertices.push_back(x2);
		vertices.push_back(baseY);
		vertices.push_back(z2);
	}

	// ===== Disque de base =====
	for (int i = 0; i < segments; ++i) {
		float angle1 = 2.f * PI * i / segments;
		float angle2 = 2.f * PI * (i + 1) / segments;

		float x1 = radius * cos(angle1);
		float z1 = radius * sin(angle1);

		float x2 = radius * cos(angle2);
		float z2 = radius * sin(angle2);

		// Centre base
		vertices.push_back(0.f);
		vertices.push_back(baseY);
		vertices.push_back(0.f);

		// Bord 2
		vertices.push_back(x2);
		vertices.push_back(baseY);
		vertices.push_back(z2);

		// Bord 1
		vertices.push_back(x1);
		vertices.push_back(baseY);
		vertices.push_back(z1);
	}
	
	// ===== OpenGL buffers =====
	glGenVertexArrays(1, &_VAO);
	glGenBuffers(1, &_VBO);

	glBindVertexArray(_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, _VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Renderer::initBox() {
	float s = _boxSize;
	float vertices[] = {
		// Back
		-s,-s,-s,	 s,-s,-s,
		 s,-s,-s,	 s, s,-s,
		 s, s,-s,	-s, s,-s,
		-s, s,-s,	-s,-s,-s,
		// Front
		-s,-s, s,	 s,-s, s,
		 s,-s, s,	 s, s, s,
		 s, s, s,	-s,	s, s,
		-s, s, s,	-s,-s, s,
		// Connections
		-s,-s,-s,	-s,-s, s,
		 s,-s,-s,	 s,-s, s,
		 s, s,-s,	 s, s, s,
		-s, s,-s,	-s,	s, s
	};
	
	// ===== OpenGL buffers =====
	glGenVertexArrays(1, &_BoxVAO);
	glGenBuffers(1, &_BoxVBO);

	glBindVertexArray(_BoxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, _BoxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Renderer::initSphere() {
	float radius = 0.5f;
	int stacks = 32, sectors = 64;
	std::vector<float> vertices;
	std::vector<unsigned int> indices;

	const float PI = 3.1415926535f;

	// Sommets + normales
	for(int i = 0; i < stacks + 1; i++) {
		float phi = PI * i / stacks;
		float r = radius * sin(phi);
		float y = radius * cos(phi);

		for (int j = 0; j < sectors + 1; j++) {
			float theta = 2 * PI * j / sectors;
			float x = r * cos(theta);
			float z = r * sin(theta);

			// Position
			vertices.push_back(x); vertices.push_back(y); vertices.push_back(z);

			// Normale (position normalisée)
			vertices.push_back(x / radius); vertices.push_back(y / radius); vertices.push_back(z / radius);
		}
	}	
	
	// Indices
	for(int i = 0; i < stacks; i++) {
		int k1 = i * (sectors + 1);
		int k2 = k1 + sectors + 1;

		for(int j = 0; j < sectors; j++, k1++, k2++) {
			indices.push_back(k1); indices.push_back(k2); indices.push_back(k1 + 1);
			indices.push_back(k1 + 1); indices.push_back(k2); indices.push_back(k2 + 1);
		}
	}
	_sphereIndexCount = indices.size();
	
	// ===== OpenGL buffers =====
	glGenVertexArrays(1, &_SphereVAO);
	glGenBuffers(1, &_SphereVBO);
	
	glBindVertexArray(_SphereVAO);
	glBindBuffer(GL_ARRAY_BUFFER, _SphereVBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size()* sizeof(float), vertices.data(), GL_STATIC_DRAW);
	
	glGenBuffers(1, &_SphereEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _SphereEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

	// Position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Normales
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);	
}

void Renderer::initCube() {
	float s = 0.5f;
	
	float vertices[] = {
		// ===== Face avant (normale : 0,0,1) =====
		-s,-s, s,  0,0,1,
		s,-s, s,  0,0,1,
		s, s, s,  0,0,1,
		-s, s, s,  0,0,1,

		// ===== Face arrière (0,0,-1) =====
		-s,-s,-s,  0,0,-1,
		-s, s,-s,  0,0,-1,
		s, s,-s,  0,0,-1,
		s,-s,-s,  0,0,-1,

		// ===== Face gauche (-1,0,0) =====
		-s,-s,-s, -1,0,0,
		-s,-s, s, -1,0,0,
		-s, s, s, -1,0,0,
		-s, s,-s, -1,0,0,

		// ===== Face droite (1,0,0) =====
		s,-s,-s,  1,0,0,
		s, s,-s,  1,0,0,
		s, s, s,  1,0,0,
		s,-s, s,  1,0,0,

		// ===== Face haut (0,1,0) =====
		-s, s,-s,  0,1,0,
		-s, s, s,  0,1,0,
		s, s, s,  0,1,0,
		s, s,-s,  0,1,0,

		// ===== Face bas (0,-1,0) =====
		-s,-s,-s,  0,-1,0,
		s,-s,-s,  0,-1,0,
		s,-s, s,  0,-1,0,
		-s,-s, s,  0,-1,0
	};

	unsigned int indices[] = {
		0,1,2,  0,2,3,
		4,5,6,  4,6,7,
		8,9,10, 8,10,11,
		12,13,14, 12,14,15,
		16,17,18, 16,18,19,
		20,21,22, 20,22,23
	};

	_cubeIndexCount = sizeof(indices) / sizeof(unsigned int);

	// ===== OpenGL =====
	glGenVertexArrays(1, &_CubeVAO);
	glGenBuffers(1, &_CubeVBO);
	glGenBuffers(1, &_CubeEBO);

	glBindVertexArray(_CubeVAO);

	glBindBuffer(GL_ARRAY_BUFFER, _CubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _CubeEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Normale
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}

void Renderer::initTorus() {
	float R = 0.7f;     // rayon principal
	float r = 0.25f;    // rayon du tube
	int majorSeg = 64;  // autour de l'anneau
	int minorSeg = 32;  // autour du tube

	std::vector<float> vertices;
	std::vector<unsigned int> indices;

	const float PI = 3.1415926535f;

	// ===== Sommets =====
	for (int i = 0; i <= majorSeg; i++) {
		float theta = 2.0f * PI * i / majorSeg;
		float cosT = cos(theta);
		float sinT = sin(theta);

		for (int j = 0; j <= minorSeg; j++) {
			float phi = 2.0f * PI * j / minorSeg;
			float cosP = cos(phi);
			float sinP = sin(phi);

			// Position
			float x = (R + r * cosP) * cosT;
			float y = r * sinP;
			float z = (R + r * cosP) * sinT;

			// Centre du tube
			float cx = R * cosT;
			float cy = 0.0f;
			float cz = R * sinT;

			// Normale
			float nx = x - cx;
			float ny = y - cy;
			float nz = z - cz;
			float len = sqrt(nx*nx + ny*ny + nz*nz);
			nx /= len; ny /= len; nz /= len;

			// Stockage
			vertices.push_back(x);
			vertices.push_back(y);
			vertices.push_back(z);
			vertices.push_back(nx);
			vertices.push_back(ny);
			vertices.push_back(nz);
		}
	}

	// ===== Indices =====
	for (int i = 0; i < majorSeg; i++) {
		for (int j = 0; j < minorSeg; j++) {
			int a = i * (minorSeg + 1) + j;
			int b = (i + 1) * (minorSeg + 1) + j;

			indices.push_back(a);
			indices.push_back(b);
			indices.push_back(a + 1);

			indices.push_back(a + 1);
			indices.push_back(b);
			indices.push_back(b + 1);
		}
	}

	_toreIndexCount = indices.size();

	// ===== OpenGL =====
	glGenVertexArrays(1, &_ToreVAO);
	glGenBuffers(1, &_ToreVBO);
	glGenBuffers(1, &_ToreEBO);

	glBindVertexArray(_ToreVAO);

	glBindBuffer(GL_ARRAY_BUFFER, _ToreVBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ToreEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

	// Position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Normales
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}

/************************************************************************
 * Rendering
 * **********************************************************************/

void Renderer::render(GLuint ssbo, glm::mat4 mvp, const std::vector<Sphere>& spheres,
						const std::vector<Cube>& cubes, const std::vector<Tore>& tores) {
	glClearColor(0.192f, 0.302f, 0.475f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Boid
	glUseProgram(_shaderProgram);
	GLint uniform_loc = glGetUniformLocation(_shaderProgram, "uMVP");
	glUniformMatrix4fv(uniform_loc, 1, GL_FALSE, glm::value_ptr(mvp));
	
	glBindVertexArray(_VAO);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);
	glDrawArraysInstanced(GL_TRIANGLES, 0, 120, _numBoids);

	glUseProgram(_boxShaderProgram);
	
	// Light
	glm::vec3 light = glm::vec3(10.0f, 5.0f, 10.0f);
	uniform_loc = glGetUniformLocation(_boxShaderProgram, "uLightDir");
	glUniform3f(uniform_loc, light.x, light.y, light.z);
	
	// Box
	glm::mat4 model = glm::mat4(1.0f);
	uniform_loc = glGetUniformLocation(_boxShaderProgram, "uMVP");
	glUniformMatrix4fv(uniform_loc, 1, GL_FALSE, glm::value_ptr(mvp));
	uniform_loc = glGetUniformLocation(_boxShaderProgram, "uModel");
	glUniformMatrix4fv(uniform_loc, 1, GL_FALSE, glm::value_ptr(model));
	
	glBindVertexArray(_BoxVAO);
	glDrawArrays(GL_LINES, 0, 24);

	// Sphere
	for (const auto& sphere : spheres) {
		glm::vec3 col = glm::vec3(0.9f, 0.2f, 0.2f);
		glm::vec3 pos = glm::vec3(sphere.positionRadius);
		float radius = sphere.positionRadius.w;
		
		glm::mat4 sphereModel = glm::translate(glm::mat4(1.0f), pos);
		sphereModel = glm::scale(sphereModel, glm::vec3(radius));
		glm::mat4 sphereMVP = mvp * sphereModel;
		
		uniform_loc = glGetUniformLocation(_boxShaderProgram, "uMVP");
		glUniformMatrix4fv(uniform_loc, 1, GL_FALSE, glm::value_ptr(sphereMVP));
		uniform_loc = glGetUniformLocation(_boxShaderProgram, "uModel");
		glUniformMatrix4fv(uniform_loc, 1, GL_FALSE, glm::value_ptr(sphereModel));
		uniform_loc = glGetUniformLocation(_boxShaderProgram, "uColor");
		glUniform3f(uniform_loc, col.x, col.y, col.z);
		
		glBindVertexArray(_SphereVAO);
		glDrawElements(GL_TRIANGLES, _sphereIndexCount, GL_UNSIGNED_INT, 0);
	}

	// Cube
	for (const auto& cube : cubes) {
		glm::vec3 col = glm::vec3(0.2f, 0.2f, 0.9f);
		glm::vec3 center = (glm::vec3(cube.min) + glm::vec3(cube.max)) * 0.5f;
		glm::vec3 size   = glm::vec3(cube.max) - glm::vec3(cube.min);
		
		glm::mat4 cubeModel = glm::translate(glm::mat4(1.0f), center);
		cubeModel = glm::scale(cubeModel, size);
		glm::mat4 cubeMVP = mvp * cubeModel;

		uniform_loc = glGetUniformLocation(_boxShaderProgram, "uMVP");
		glUniformMatrix4fv(uniform_loc, 1, GL_FALSE, glm::value_ptr(cubeMVP));
		uniform_loc = glGetUniformLocation(_boxShaderProgram, "uModel");
		glUniformMatrix4fv(uniform_loc, 1, GL_FALSE, glm::value_ptr(cubeModel));
		uniform_loc = glGetUniformLocation(_boxShaderProgram, "uColor");
		glUniform3f(uniform_loc, col.x, col.y, col.z);
		
		glBindVertexArray(_CubeVAO);
		glDrawElements(GL_TRIANGLES, _cubeIndexCount, GL_UNSIGNED_INT, 0);
	}

	// Tore
	for (const auto& tore : tores) {
		glm::vec3 col = glm::vec3(0.4f, 0.8f, 0.1f);
		glm::vec3 pos = glm::vec3(tore.position);

		glm::vec3 scaleVec(
			tore.radii.x / 0.7f,   // scale de R
			tore.radii.y / 0.25f,  // scale de r
			tore.radii.x / 0.7f    // X et Z suivent R
		);
		glm::mat4 toreModel = glm::translate(glm::mat4(1.0f), pos);
		toreModel = glm::scale(toreModel, scaleVec);
		glm::mat4 toreMVP = mvp * toreModel;

		uniform_loc = glGetUniformLocation(_boxShaderProgram, "uMVP");
		glUniformMatrix4fv(uniform_loc, 1, GL_FALSE, glm::value_ptr(toreMVP));
		uniform_loc = glGetUniformLocation(_boxShaderProgram, "uModel");
		glUniformMatrix4fv(uniform_loc, 1, GL_FALSE, glm::value_ptr(toreModel));
		uniform_loc = glGetUniformLocation(_boxShaderProgram, "uColor");
		glUniform3f(uniform_loc, col.x, col.y, col.z);

		glBindVertexArray(_ToreVAO);
		glDrawElements(GL_TRIANGLES, _toreIndexCount, GL_UNSIGNED_INT, 0);
	}
}


