/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Renderer.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/18 15:25:40 by lde-merc          #+#    #+#             */
/*   Updated: 2026/03/20 19:34:55 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

// OpenGL
#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Exception.hpp"
#include <fstream>
#include <sstream>
#include <vector>

class Renderer {
	public:
		Renderer();
		~Renderer();

		void init(int, int, int);
		void initShaders();
		void createBuffers();
		void initBox();
		
		void render(GLuint, glm::mat4);
		void resize(int, int);

		const GLuint& getShaderProg() const { return _shaderProgram; };

	private:
		int _width;
		int _height;
		int _numBoids;
		float _boxSize = 10.0f;
		
		// OpenGL parameters
		GLuint	_shaderProgram;
		GLuint	_boxShaderProgram;
		GLuint	_VAO;
		GLuint	_VBO;
		
		GLuint	_BoxVAO;
		GLuint	_BoxVBO;
};
