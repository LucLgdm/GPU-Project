/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Application.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/18 11:58:16 by lde-merc          #+#    #+#             */
/*   Updated: 2026/03/20 16:31:56 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <memory>

#include "Exception.hpp"
#include "BoidSimulation.hpp"
#include "Renderer.hpp"
#include "Shader.hpp"
#include "Camera.hpp"

struct GLFWwindow;
struct GLFWmonitor;

class Application {
	public:
		Application();
		~Application();

		void init();
		void run();
		
		void handleKey();
		void toggleFullscreen();

	private:
		GLFWwindow *_window;
		std::unique_ptr<BoidSimulation> _simulation;
		std::unique_ptr<Renderer> _renderer;
		Camera	_camera;

		// Window, fullscreen
		int _width = 1600;
		int _height = 1200;
		int _currentWidth, _currentHeight;
		bool _fullscreen = false;
		int _windowedX, _windowedY, _windowedWidth, _windowedHeight;
		GLFWmonitor *_currentMonitor;
		void initGLFW();
};