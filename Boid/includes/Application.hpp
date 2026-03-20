/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Application.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/18 11:58:16 by lde-merc          #+#    #+#             */
/*   Updated: 2026/03/20 12:49:43 by lde-merc         ###   ########.fr       */
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

class Application {
	public:
		Application();
		~Application();

		void init();
		void run();
		void handleKey();

	private:
		GLFWwindow *_window;
		std::unique_ptr<BoidSimulation> _simulation;
		std::unique_ptr<Renderer> _renderer;
		Camera	_camera;

		int _width = 1600;
		int _height = 1200;

		void initGLFW();
};