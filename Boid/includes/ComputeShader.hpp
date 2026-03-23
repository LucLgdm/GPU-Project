/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ComputeShader.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/20 11:00:14 by lde-merc          #+#    #+#             */
/*   Updated: 2026/03/23 12:08:51 by lde-merc         ###   ########.fr       */
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

		void updateBoundSize(float size) {_boundSize = size; };

		const float& getSepRad() const { return _sepRadius; };
		const float& getAlignRad() const { return _alignRadius; };
		const float& getCoheRad() const { return _coheRadius; };
		
		const float& getSepWei() const { return _sepWeight; };
		const float& getAlignWei() const { return _alignWeight; };
		const float& getCoheWei() const { return _coheWeight; };

		const float& getMaxSpeed() const { return _maxSpeed; };
		const float& getMinSpeed() const { return _minSpeed; };
		const float& getTurnSpeed() const { return _turnSpeed; };

		void setSepRad(float rad) { _sepRadius = rad; };
		void setAlignRad(float rad) { _alignRadius = rad; };
		void setCoheRad(float rad) { _coheRadius = rad; };
		
		void setSepWei(float wei) { _sepWeight = wei; };
		void setAlignWei(float wei) { _alignWeight = wei; };
		void setCoheWei(float wei) { _coheWeight = wei; };

		void setMaxSpeed(float speed) { _maxSpeed = speed; };
		void setMinSpeed(float speed) { _minSpeed = speed; };
		void setTurnSpeed(float speed) { _turnSpeed = speed; };
		
	private:
		// Rules
		float _sepRadius = 0.75f;
		float _sepWeight = 1.5f;

		float _alignRadius = 0.75f;
		float _alignWeight = 1.5f;
		
		float _coheRadius = 0.75f;
		float _coheWeight = 1.5f;
		
		// Life
		float _maxSpeed = 13.0f;
		float _minSpeed = 0.01f;
		float _boundSize = 50.0f;
		float _turnSpeed = 20.0f;
		
		GLuint _program;
};