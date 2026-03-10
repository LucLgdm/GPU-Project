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
	if (glfwGetKey(_window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
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

	// --- Fullscreen toggle ---
	if (glfwGetKey(_window, GLFW_KEY_F11) == GLFW_PRESS) {
		fullscreen = !fullscreen;
		
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

	// --- Camera movement ---
	handleKeyCamera(_window, _raycaster->getCamera());
}

void Application::handleKeyCamera(GLFWwindow* window, Camera& camera) {
	const float moveSpeed = 0.01f;
	const float rotSpeed = 0.03f;

	// deplacements
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		camera.x += camera.dirX * moveSpeed;
		camera.y += camera.dirY * moveSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		camera.x -= camera.dirX * moveSpeed;
		camera.y -= camera.dirY * moveSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		camera.x -= camera.planeX * moveSpeed;
		camera.y -= camera.planeY * moveSpeed;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		camera.x += camera.planeX * moveSpeed;
		camera.y += camera.planeY * moveSpeed;
	}

	// Rotation
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		float oldDirX = camera.dirX;
		camera.dirX = camera.dirX * cos(rotSpeed) - camera.dirY * sin(rotSpeed);
		camera.dirY = oldDirX * sin(rotSpeed) + camera.dirY * cos(rotSpeed);
		float oldPlaneX = camera.planeX;
		camera.planeX = camera.planeX * cos(rotSpeed) - camera.planeY * sin(rotSpeed);
		camera.planeY = oldPlaneX * sin(rotSpeed) + camera.planeY * cos(rotSpeed);
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		float oldDirX = camera.dirX;
		camera.dirX = camera.dirX * cos(-rotSpeed) - camera.dirY * sin(-rotSpeed);
		camera.dirY = oldDirX * sin(-rotSpeed) + camera.dirY * cos(-rotSpeed);
		float oldPlaneX = camera.planeX;
		camera.planeX = camera.planeX * cos(-rotSpeed) - camera.planeY * sin(-rotSpeed);
		camera.planeY = oldPlaneX * sin(-rotSpeed) + camera.planeY * cos(-rotSpeed);
	}

}
