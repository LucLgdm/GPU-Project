/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ComputeShader.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/20 11:00:14 by lde-merc          #+#    #+#             */
/*   Updated: 2026/03/20 12:06:18 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Exception.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <fstream>
#include <sstream>
#include <cmath>


class ComputeShader {
	public:
		ComputeShader();
		~ComputeShader();

		void init(const char*);
		void dispatch(uint, float);
		void use();

	private:
		GLuint _program;
};