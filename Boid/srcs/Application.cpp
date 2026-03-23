/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Application.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/18 11:58:12 by lde-merc          #+#    #+#             */
/*   Updated: 2026/03/23 15:56:27 by lde-merc         ###   ########.fr       */
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
	_camera.init(_window, _width, _height);
	_imguiLayer.init(_window);
	glfwSetWindowUserPointer(_window, this);
	std::cout << "\033[32m	Initialisztion completed !\033[0m" << std::endl;
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
	glEnable(GL_DEPTH_TEST);
	
	setCallbacks();
}


void Application::setCallbacks() {
	// Callbacks for Camera rotation
	glfwSetMouseButtonCallback(_window, [](GLFWwindow* win, int button, int action, int mods) {
		Application* app = reinterpret_cast<Application*>(glfwGetWindowUserPointer(win));
		if (button == GLFW_MOUSE_BUTTON_RIGHT) {
			if (action == GLFW_PRESS)
				app->_camera.beginRotate();
			else if (action == GLFW_RELEASE)
				app->_camera.endRotate();
		}
	});

	glfwSetCursorPosCallback(_window, [](GLFWwindow* win, double xpos, double ypos) {
		Application* app = reinterpret_cast<Application*>(glfwGetWindowUserPointer(win));
		app->_camera.processMouseMove(static_cast<float>(xpos), static_cast<float>(ypos));
	});
}

/************************************************************************
 * Main loop
 * **********************************************************************/

void Application::run() {
	std::cout << "\033[33m	Let's play!\033[0m" << std::endl;
	float currentTime = glfwGetTime();
	while (!glfwWindowShouldClose(_window)) {
		float deltaTime = glfwGetTime() - currentTime;
		currentTime = glfwGetTime();
		handleKey();
		_camera.update(_window);
        
		// Update simulation
		float ratio = static_cast<float>(_width) / static_cast<float>(_height);
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 mvp = _camera.getProjection(ratio) * _camera.getView() * model;
		_simulation->update(deltaTime);
		_renderer->render(_simulation->getSsbo(), mvp, _simulation->getSphere());
		_imguiLayer.render(*_simulation, *_renderer);
        glfwSwapBuffers(_window);
        glfwPollEvents();	
	}
	_imguiLayer.shutdown();
}


/************************************************************************
 * Input handling
 * **********************************************************************/

void Application::handleKey() {
	static bool f11Pressed = false;

	// --- Fullscreen toggle ---
	bool f11Now = (glfwGetKey(_window, GLFW_KEY_F11) == GLFW_PRESS);
	if (f11Now && !f11Pressed && !_fullscreen) {
		toggleFullscreen();   // ta fonction qui gère le passage full screen
	}
	f11Pressed = f11Now;

		// --- Escape to quit if not in fullscreen mode ---
	static bool escPressed = false;
	bool escNow = (glfwGetKey(_window, GLFW_KEY_ESCAPE) == GLFW_PRESS);

	if (escNow && !escPressed) {
		GLFWmonitor* monitor = glfwGetWindowMonitor(_window);

		if (monitor) {
			toggleFullscreen();
		} else {
			glfwSetWindowShouldClose(_window, true);
		}
	}
	escPressed = escNow;
}

void Application::toggleFullscreen() {
	_fullscreen = !_fullscreen;
	
	if (_fullscreen) {
		// Save position and size of the window
		glfwGetWindowPos(_window, &_windowedX, &_windowedY);
		glfwGetWindowSize(_window, &_windowedWidth, &_windowedHeight);
		
		// Fullscreen
		_currentMonitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(_currentMonitor);
		_currentWidth = mode->width;
		_currentHeight = mode->height;
		
		glfwSetWindowMonitor(_window, _currentMonitor, 0, 0, mode->width,
				mode->height, mode->refreshRate);

		// Update viewport and camera aspect ratio
		glViewport(0, 0, mode->width, mode->height);
		_camera.updateProjectionMatrix(mode->width, mode->height);
		glfwSetCursorPos(_window, mode->width / 2.0f, mode->height / 2.0f);
		_camera.resetMouse(mode->width / 2.0f, mode->height / 2.0f);
	} else {
		// Back to windowed mode
		_currentWidth = _windowedWidth;
		_currentHeight = _windowedHeight;
		
		glfwSetWindowMonitor(_window, nullptr, _windowedX, _windowedY, _width, _height, 0);

		// Update viewport and camera aspect ratio
		glViewport(0, 0, _windowedWidth, _windowedHeight);
		_camera.updateProjectionMatrix(_windowedWidth, _windowedHeight);
		glfwSetCursorPos(_window, _windowedWidth / 2.0f, _windowedHeight / 2.0f);
		_camera.resetMouse(_windowedWidth / 2.0f, _windowedHeight / 2.0f);
	}
}