/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Renderer.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/18 15:25:40 by lde-merc          #+#    #+#             */
/*   Updated: 2026/03/24 14:39:39 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

// OpenGL
#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Exception.hpp"
#include <fstream>
#include <sstream>
#include <vector>

#include "BoidSimulation.hpp"

class Renderer {
	public:
		Renderer();
		~Renderer();

		void init(int, int, int);
		void initShaders();
		void createBuffers();
		void initBox();
		void initSphere();
		void initCube();
		void initTorus();
		
		void render(GLuint, glm::mat4, const std::vector<Sphere>&, const std::vector<Cube>&, const std::vector<Tore>&);
		void resize(int, int);

		void updateBoxSize(float size) {_boxSize = size; };
		
		const GLuint& getShaderProg() const { return _shaderProgram; };
		const float& getBoxSize() const { return _boxSize; };


	private:
		int _width;
		int _height;
		int _numBoids;
		float _boxSize = 5.0f;
		
		// OpenGL parameters
		GLuint	_shaderProgram;
		GLuint	_boxShaderProgram;
		GLuint	_VAO;
		GLuint	_VBO;
		
		GLuint	_BoxVAO;
		GLuint	_BoxVBO;

		GLuint _SphereVAO, _SphereVBO, _SphereEBO;
		int _sphereIndexCount;

		GLuint _CubeVAO, _CubeVBO, _CubeEBO;
		int _cubeIndexCount;

		GLuint _ToreVAO, _ToreVBO, _ToreEBO;
		int _toreIndexCount;
};
