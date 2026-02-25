/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Application.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/23 17:47:22 by lde-merc          #+#    #+#             */
/*   Updated: 2026/02/25 15:02:09 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

// OpenGL
#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include "Exception.hpp"

#include "World.hpp"
#include "Renderer.hpp"
#include "Raycaster.hpp"

class Application {
	public:
		Application();
		~Application();
		void cleanup();
	
		// Initialization
		void init(int, char **);
		void checkInput(int, char **);
		void initShader();

		// Main loop
		void run();

		void handleKey();
	private:
		// Window parameters
		GLFWwindow *_window;
		std::string _mapName;
		int 		_width = 800;
		int 		_height = 600;

		// OpenGL parameters
		GLuint _shaderProgram;
	
		// Functions
		void initGLFW();
		void initOpenGL();
};