/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Renderer.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/25 17:31:28 by lde-merc          #+#    #+#             */
/*   Updated: 2026/03/09 16:53:41 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// This class will handle all the rendering, using OpenGL. 
// It will take care of creating the shaders, the buffers, and rendering the scene.
// It's pure rendering, it should not contain any game logic.

#pragma once

// OpenGL
#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <sstream>

#include "Exception.hpp"


class cudaGraphicsResource; // Forward declaration for CUDA graphics resource
class uchar4;

class Renderer {
	public:
		Renderer();
		~Renderer();

		void init(int, int);
		void initShaders();
		void initOpenGL();
		void createBuffers();
		void createTextures();
		
		void render();

		GLuint getPBO() const { return _PBO; }
		cudaGraphicsResource* getcudaPBO() const { return _cudaPBO; }

		uchar4* mapPBO();
		void unmapPBO();
		
	private:
		int _width;
		int _height;
		
		// OpenGL parameters
		GLuint					 _shaderProgram;
		GLuint					 _VAO;
		GLuint					 _VBO;
		GLuint					 _texture;
		GLuint					 _PBO;		// Pixel Buffer Object
		cudaGraphicsResource*	 _cudaPBO;	// CUDA Pixel Buffer Object
};
