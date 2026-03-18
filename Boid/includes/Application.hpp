/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Application.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/18 11:58:16 by lde-merc          #+#    #+#             */
/*   Updated: 2026/03/18 12:28:43 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

// OpenGL
#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Exception.hpp"
#include <iostream>

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
		int _width = 800;
		int _height = 600;

		
		void initGLFW();
};