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

#include <iostream>
#include <fstream>
#include <sstream>

#include <cmath>
#include <memory>
#include "Exception.hpp"

#include "Renderer.hpp"
#include "Raycaster.hpp"

class Application {
	public:
		Application();
		~Application();
	
		// Initialization
		void init(int, char **);

		// Main loop
		void run();
		void handleKey();
		void handleKeyCamera(GLFWwindow*, Camera&);
	private:
		// Window parameters
		GLFWwindow *_window;
		int 		_width = 1600;
		int 		_height = 1200;

		Renderer 	_renderer;
		std::unique_ptr<Raycaster> _raycaster;

		// Functions
		void initGLFW();
};