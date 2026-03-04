/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Renderer.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/04 12:10:37 by lde-merc          #+#    #+#             */
/*   Updated: 2026/03/04 16:19:09 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cuda_gl_interop.h>

#include "Renderer.hpp"

Renderer::Renderer() : _shaderProgram(0), _VAO(0), _VBO(0), _texture(0), _PBO(0), _cudaPBO(0) {
}

Renderer::~Renderer() {
	if (_PBO) glDeleteBuffers(1, &_PBO);
	if (_cudaPBO) cudaGraphicsUnregisterResource(_cudaPBO);
	if (_texture) glDeleteTextures(1, &_texture);
	if (_VAO) glDeleteVertexArrays(1, &_VAO);
	if (_VBO) glDeleteBuffers(1, &_VBO);
	if (_shaderProgram) glDeleteProgram(_shaderProgram);
}

/************************************************************************
 * Initialization
 * **********************************************************************/

void Renderer::init(int width, int height) {
	_width = width;
	_height = height;
	initShaders();
	createBuffers();
	createTextures();
}

static std::string readFile(const char* path) {
	std::ifstream file(path);
	if (!file.is_open())
		throw std::runtime_error(std::string("Cannot open shader file: ") + path);

	std::stringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
}

void Renderer::initShaders() {
	const char* vPath = "shaders/vertex.glsl";
	const char* fPath = "shaders/fragment.glsl";
	
	std::string vCode = readFile(vPath);
	std::string fCode = readFile(fPath);
	
	const char* vSrc = vCode.c_str();
	const char* fSrc = fCode.c_str();

	GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vShader, 1, &vSrc, nullptr);
	glCompileShader(vShader);

	GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fShader, 1, &fSrc, nullptr);
	glCompileShader(fShader);
	
	_shaderProgram = glCreateProgram();

	glAttachShader(_shaderProgram, vShader);
	glAttachShader(_shaderProgram, fShader);
	glLinkProgram(_shaderProgram);

	glDeleteShader(vShader);
	glDeleteShader(fShader);
}

void Renderer::createBuffers() {
	// Vertex data for a full-screen quad
	float vertices[] = {
		-1.f, -1.f,  0.f, 0.f,
		1.f, -1.f,  1.f, 0.f,
		1.f,  1.f,  1.f, 1.f,
		-1.f, -1.f,  0.f, 0.f,
		1.f,  1.f,  1.f, 1.f,
		-1.f,  1.f,  0.f, 1.f,
	};
	glGenVertexArrays(1, &_VAO);
	glGenBuffers(1, &_VBO);

	glBindVertexArray(_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, _VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Create Pixel Buffer Object (PBO)
	glGenBuffers(1, &_PBO);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, _PBO);
	glBufferData(GL_PIXEL_UNPACK_BUFFER, _width * _height * sizeof(uint32_t), nullptr, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

	// Register PBO with CUDA
	cudaGraphicsGLRegisterBuffer(&_cudaPBO, _PBO, cudaGraphicsMapFlagsWriteDiscard);
}

void Renderer::createTextures() {
	glGenTextures(1, &_texture);
	glBindTexture(GL_TEXTURE_2D, _texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);
}

/************************************************************************
 * Rendering
 * **********************************************************************/

void Renderer::render() {
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, _PBO);
	
	glBindTexture(GL_TEXTURE_2D, _texture);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, _width, _height, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

	glClear(GL_COLOR_BUFFER_BIT);
	
	glUseProgram(_shaderProgram);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _texture);
	
	glUniform1i(glGetUniformLocation(_shaderProgram, "uTexture"), 0);
	
	glBindVertexArray(_VAO);

	glDrawArrays(GL_TRIANGLES, 0, 6);
}