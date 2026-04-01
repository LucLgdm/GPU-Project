/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Application.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/25 17:46:02 by lde-merc          #+#    #+#             */
/*   Updated: 2026/04/01 12:48:23 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <memory>

#include "Exception.hpp"
#include "Renderer.hpp"
#include "Camera.hpp"
#include "Compute.hpp"
#include "Scene.hpp"


struct GLFWwindow;
struct GLFWmonitor;

class Application {
	public:
		Application();
		~Application();

		void init(char *);
		void run();
		
		void handleKey();
		void toggleFullscreen();

	private:
		GLFWwindow *_window;
		std::unique_ptr<Renderer> _renderer;
		std::unique_ptr<Compute> _computer;
		std::unique_ptr<Scene> _scene;

		Camera	_camera;
		// ImGuiLayer	_imguiLayer;

		// Window, fullscreen
		int _width = 1600;
		int _height = 1200;
		int _currentWidth, _currentHeight;
		bool _fullscreen = false;
		int _windowedX, _windowedY, _windowedWidth, _windowedHeight;
		GLFWmonitor *_currentMonitor;
		
		void initGLFW();
		void setCallbacks();
};