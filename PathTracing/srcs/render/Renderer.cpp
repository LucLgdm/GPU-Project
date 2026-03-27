/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Renderer.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/25 18:01:30 by lde-merc          #+#    #+#             */
/*   Updated: 2026/03/26 17:39:04 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Renderer.hpp"
#include <cuda_gl_interop.h>

Renderer::Renderer(): _width(0), _height(0), _shaderProgram(0),
						_quadVAO(0), _quadVBO(0), _PBO(0) { }

Renderer::~Renderer() { 
	if (_quadVAO) glDeleteVertexArrays(1, &_quadVAO);
	if (_quadVBO) glDeleteBuffers(1, &_quadVBO);
	if (_shaderProgram) glDeleteProgram(_shaderProgram);
	if (_cudaPBO) cudaGraphicsUnregisterResource(_cudaPBO);
}

/************************************************************************
 * Initialization
 * **********************************************************************/

void Renderer::init(int width, int height) {
	_width = width;
	_height = height;
	initShaders();
	createBuffers();
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
	
	std::string vCode = readFile(vPath);
	std::string fCode = readFile(fPath);
	
	const char* vSrc = vCode.c_str();
	const char* fSrc = fCode.c_str();

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

	glAttachShader(_shaderProgram, vShader);
	glAttachShader(_shaderProgram, fShader);
	glLinkProgram(_shaderProgram);
	
	glDeleteShader(vShader);
	glDeleteShader(fShader);
}

void Renderer::createBuffers() {

	// =========================
	// Fullscreen Quad
	// =========================
	float quadVertices[] = {
		// positions   // uvs
		-1.f, -1.f,    0.f, 0.f,
		1.f, -1.f,    1.f, 0.f,
		-1.f,  1.f,    0.f, 1.f,
		1.f,  1.f,    1.f, 1.f
	};

	glGenVertexArrays(1, &_quadVAO);
	glGenBuffers(1, &_quadVBO);

	glBindVertexArray(_quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, _quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(2*sizeof(float)));

	glBindVertexArray(0);

	// =========================
	// Texture (CUDA output)
	// =========================
	glGenTextures(1, &_texture);
	glBindTexture(GL_TEXTURE_2D, _texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);

	// =========================
	// PBO (CUDA interop)
	// =========================
	glGenBuffers(1, &_PBO);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, _PBO);
	glBufferData(GL_PIXEL_UNPACK_BUFFER, _width * _height * 4, nullptr, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

	// =========================
	// CUDA Registration
	// =========================
	cudaGraphicsGLRegisterBuffer(&_cudaPBO, _PBO, cudaGraphicsMapFlagsWriteDiscard);
}

/************************************************************************
 * Rendering
 * **********************************************************************/

void Renderer::render() {
	// =========================
	// Copy PBO → Texture
	// =========================
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, _PBO);
	glBindTexture(GL_TEXTURE_2D, _texture);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, _width, _height, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

	// =========================
	// Draw fullscreen quad
	// =========================
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(_shaderProgram);
	glUniform1i(glGetUniformLocation(_shaderProgram, "uTex"), 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _texture);

	glBindVertexArray(_quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}


/************************************************************************
 * Cuda 
 * **********************************************************************/

uchar4* Renderer::mapPBO() {
	uchar4* devPtr = nullptr;
	size_t size = 0;
	cudaGraphicsMapResources(1, &_cudaPBO, 0);
	cudaGraphicsResourceGetMappedPointer((void**)&devPtr, &size, _cudaPBO);
	return devPtr;
}

void Renderer::unmapPBO() {
	cudaGraphicsUnmapResources(1, &_cudaPBO, 0);
}