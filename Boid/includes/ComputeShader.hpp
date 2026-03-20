/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ComputeShader.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/20 11:00:14 by lde-merc          #+#    #+#             */
/*   Updated: 2026/03/20 16:42:18 by lde-merc         ###   ########.fr       */
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
		// Rules
		float _sepRadius = 0.75f;
		float _sepWeight = 1.5f;

		float _alignRadius = 0.75f;
		float _alignWeight = 1.5f;
		
		float _coheRadius = 0.75f;
		float _coheWeight = 1.5f;
		
		// Life
		float _maxSpeed = 3.0f;
		float _minSpeed = 0.01f;
		float _boundSize = 2.0f;
		float _turnSpeed = 0.5f;
		
		GLuint _program;
};