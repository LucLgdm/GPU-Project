/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Application.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/23 17:47:48 by lde-merc          #+#    #+#             */
/*   Updated: 2026/02/25 15:02:02 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Application.hpp"
#include "Raycaster.hpp"
#include <cuda_runtime.h>

Application::Application() { }

Application::~Application() { 
	glfwDestroyWindow(_window);
	glfwTerminate();
}


/************************************************************************
 * Initialization
 * **********************************************************************/

void Application::init(int argc, char **argv) {
	std::cout << "\033[33m	Initializing Application...\033[0m" << std::endl;
	if (argc != 2)
		throw inputError("	Usage: ./rc <map_file>");
	initGLFW();
	_renderer.init(_width, _height);
	_raycaster = std::make_unique<Raycaster>(argv[1]);
	std::cout << "\033[33m	Application initialized!\033[0m" << std::endl;
}

void Application::initGLFW() {
	if (!glfwInit())
		throw glfwError("	GLFW initialization failed");

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	_window = glfwCreateWindow(_width, _height, "Ray Caster GPU", nullptr, nullptr);
	if (!_window)
		throw glfwError("	Window creation failed");

	glfwMakeContextCurrent(_window);
	glfwSwapInterval(1);

	// Charger les fonctions OpenGL avec Glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		throw openGlError("	Failed to initialize GLAD");
	
	// Configuration OpenGL
	glViewport(0, 0, _width, _height);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glFrontFace(GL_CCW);
}

/************************************************************************
 * Main loop
 * **********************************************************************/

void Application::run() {
	while (!glfwWindowShouldClose(_window)) {		
		
		handleKey();
		
		// uchar4* devPtr = _renderer.mapPBO();
        // _raycaster->update(devPtr);
        // _renderer.unmapPBO();
        
        // _renderer.render();
        
        glfwSwapBuffers(_window);
        glfwPollEvents();	
	}
}

/************************************************************************
 * Input handling
 * **********************************************************************/

void Application::handleKey() {
	// --- Escape to close the window ---
	if (glfwGetKey(_window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(_window, true);
	}
}
