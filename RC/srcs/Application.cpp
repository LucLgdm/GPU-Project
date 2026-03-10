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
	_raycaster = std::make_unique<Raycaster>(argv[1], _width, _height);
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
	std::cout << std::endl << "\033[33mLet's play!\033[0m" << std::endl;
	while (!glfwWindowShouldClose(_window)) {		
		
		handleKey();
		
		uchar4* devPtr = _renderer.mapPBO();
        _raycaster->update(devPtr);
        _renderer.unmapPBO();
        
        _renderer.render();
        
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
			_width = savedW; _height = savedH;
			_renderer.resize(_width, _height);
			_raycaster->updateFullscreen(_width, _height);
		}else {
			glfwSetWindowShouldClose(_window, true);
		}
	}
	escWasPressed = escPressed;

	// --- Fullscreen toggle ---
	static bool f11WasPressed = false;
	bool f11Pressed = glfwGetKey(_window, GLFW_KEY_F11) == GLFW_PRESS;

	if (f11Pressed && !f11WasPressed && !fullscreen) {
		fullscreen = true;
		if (fullscreen) {
			// Sauvegarder la position/taille de la fenêtre
			glfwGetWindowPos(_window, &savedX, &savedY);
			glfwGetWindowSize(_window, &savedW, &savedH);
			
			GLFWmonitor* monitor = glfwGetPrimaryMonitor();
			const GLFWvidmode* mode = glfwGetVideoMode(monitor);
			glfwSetWindowMonitor(_window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
			_width = mode->width; _height = mode->height;
			_renderer.resize(_width, _height);
			_raycaster->updateFullscreen(_width, _height);
		}
	}
	f11WasPressed = f11Pressed;
	_raycaster->move(_window);
}

