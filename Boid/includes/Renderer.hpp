/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Renderer.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/18 15:25:40 by lde-merc          #+#    #+#             */
/*   Updated: 2026/03/18 17:40:39 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

// OpenGL
#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>


#include "Exception.hpp"
#include <fstream>
#include <sstream>

class Renderer {
	public:
		Renderer();
		~Renderer();

		void init(int, int, int);
		void initShaders();
		void createBuffers();
		
		void render(GLuint);
		void resize(int, int);

	private:
		int _width;
		int _height;
		int _numBoids;
		
		// OpenGL parameters
		GLuint					 _shaderProgram;
		GLuint					 _VAO;
		GLuint					 _VBO;
};
