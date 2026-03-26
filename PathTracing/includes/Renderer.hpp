/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Renderer.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/25 18:01:32 by lde-merc          #+#    #+#             */
/*   Updated: 2026/03/26 11:22:58 by lde-merc         ###   ########.fr       */
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

class cudaGraphicsResource;
class uchar4;

class Renderer {
	public:
		Renderer();
		~Renderer();

		void init(int, int);
		void initShaders();
		void createBuffers();
		
		void render();

		GLuint getPBO() const { return _PBO; }
		cudaGraphicsResource* getcudaPBO() const { return _cudaPBO; }

		uchar4* mapPBO();
		void unmapPBO();

	private:
		int _width;
		int _height;
		
		// OpenGL parameters
		GLuint	_shaderProgram;
		GLuint _quadVAO, _quadVBO;
		GLuint _texture;
		GLuint _PBO;
		cudaGraphicsResource* _cudaPBO;
};