/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Application.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/18 11:58:12 by lde-merc          #+#    #+#             */
/*   Updated: 2026/03/18 17:42:43 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Application.hpp"

Application::Application() { }

Application::~Application() { 
	glfwDestroyWindow(_window);
	glfwTerminate();
}

/************************************************************************
 * Initialization
 * **********************************************************************/

void Application::init() {
	initGLFW();
	_simulation = std::make_unique<BoidSimulation>(1000, _width, _height);
	_renderer = std::make_unique<Renderer>();
	_renderer->init(_width, _height, 1000);
}

void Application::initGLFW() {
	if (!glfwInit())
		throw glfwError("GLFW initialization failed");

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	_window = glfwCreateWindow(_width, _height, "Boids GPU", nullptr, nullptr);
	if (!_window)
		throw glfwError("Window creation failed");

	glfwMakeContextCurrent(_window);
	glfwSwapInterval(1);

	// Charger les fonctions OpenGL avec Glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		throw openGlError("Failed to initialize GLAD");
	
	// Configuration OpenGL
	glViewport(0, 0, _width, _height);
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glFrontFace(GL_CCW);
}

/************************************************************************
 * Main loop
 * **********************************************************************/

void Application::run() {
	std::cout << "\033[33m	Let's play!\033[0m" << std::endl;
	while (!glfwWindowShouldClose(_window)) {		
		
		handleKey();
        
		// Update simulation
		// _simulation->update(0.016f); // ~60 FPS
		_renderer->render(_simulation->getSsbo());
		GLenum err = glGetError();
		if (err != GL_NO_ERROR)
			std::cout << "OpenGL error: " << err << std::endl;
        glfwSwapBuffers(_window);
        glfwPollEvents();	
	}
}

/************************************************************************
 * Input handling
 * **********************************************************************/

void Application::handleKey() {
	static bool fullscreen = false;
	static int savedX, savedY, savedW, savedH;

	// --- Escape to close the window ---
	static bool escWasPressed = false;
	bool escPressed = glfwGetKey(_window, GLFW_KEY_ESCAPE) == GLFW_PRESS;

	if (escPressed && !escWasPressed) {
		if (fullscreen) {
			// Revenir en windowed
			glfwSetWindowMonitor(_window, nullptr, savedX, savedY, savedW, savedH, 0);
			fullscreen = !fullscreen;
		}else {
			glfwSetWindowShouldClose(_window, true);
		}
	}
	escWasPressed = escPressed;
}
