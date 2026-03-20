/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Renderer.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/18 15:35:33 by lde-merc          #+#    #+#             */
/*   Updated: 2026/03/20 10:14:02 by lde-merc         ###   ########.fr       */
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
	float vertices[] = {
		0.0f,			1.0f / 75.0f,  0.0f,
		-0.3f / 75.0f,	-1.0f / 75.0f, 0.0f,
		0.3f / 75.0f, 	-1.0f / 75.0f, 0.0f
	};
	glGenVertexArrays(1, &_VAO);
	glGenBuffers(1, &_VBO);

	glBindVertexArray(_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, _VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}


/************************************************************************
 * Rendering
 * **********************************************************************/

void Renderer::render(GLuint ssbo) {
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	
	glUseProgram(_shaderProgram);
	
	glBindVertexArray(_VAO);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);
	glDrawArraysInstanced(GL_TRIANGLES, 0, 3, _numBoids);
}

void Renderer::resize(int width, int height) {
    _width = width;
    _height = height;
       
	glViewport(0, 0, _width, _height);
    // Recréer à la bonne taille
    createBuffers();
}

