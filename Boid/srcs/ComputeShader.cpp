/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ComputeShader.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/20 11:00:17 by lde-merc          #+#    #+#             */
/*   Updated: 2026/03/20 15:26:29 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ComputeShader.hpp"

ComputeShader::ComputeShader(): _program(0) {}

ComputeShader::~ComputeShader() { }

static std::string readFile(const char* path) {
	std::ifstream file(path);
	if (!file.is_open())
		throw inputError(std::string("Cannot open shader file: ") + path);

	std::stringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
}

void ComputeShader::init(const char *path) {
	std::string cSCode = readFile(path);	
	const char* csSrc = cSCode.c_str();

	GLuint cShader = glCreateShader(GL_COMPUTE_SHADER);
	glShaderSource(cShader, 1, &csSrc, nullptr);
	glCompileShader(cShader);

	GLint success;
	glGetShaderiv(cShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		char log[512];
		glGetShaderInfoLog(cShader, 512, nullptr, log);
		throw openGlError(log);
	}

	_program = glCreateProgram();
	glAttachShader(_program, cShader);
	glLinkProgram(_program);
	
	glDeleteShader(cShader);
}

void ComputeShader::dispatch(uint numBoids, float deltaTime) {
	glUseProgram(_program);
	GLint uniform_loc;
	// Differents parameters
	uniform_loc = glGetUniformLocation(_program, "uNumBoids");
	glUniform1ui(uniform_loc, numBoids);
	uniform_loc = glGetUniformLocation(_program, "uDeltaTime");
	glUniform1f(uniform_loc, deltaTime);
	uniform_loc = glGetUniformLocation(_program, "uMaxSpeed");
	glUniform1f(uniform_loc, _maxSpeed);
	uniform_loc = glGetUniformLocation(_program, "uMinSpeed");
	glUniform1f(uniform_loc, _minSpeed);
		// Boundaries
	uniform_loc = glGetUniformLocation(_program, "uBoundSize");
	glUniform1f(uniform_loc, _boundSize);
	uniform_loc = glGetUniformLocation(_program, "uTurnSpeed");
	glUniform1f(uniform_loc, _turnSpeed);
	
	// Boid rules
	uniform_loc = glGetUniformLocation(_program, "uSeparationRadius");
	glUniform1f(uniform_loc, _sepRadius);
	uniform_loc = glGetUniformLocation(_program, "uSeparationWeight");
	glUniform1f(uniform_loc, _sepWeight);
	uniform_loc = glGetUniformLocation(_program, "uAlignementRadius");
	glUniform1f(uniform_loc, _alignRadius);
	uniform_loc = glGetUniformLocation(_program, "uAlignementWeight");
	glUniform1f(uniform_loc, _alignWeight);
	uniform_loc = glGetUniformLocation(_program, "uCohesionRadius");
	glUniform1f(uniform_loc, _coheRadius);
	uniform_loc = glGetUniformLocation(_program, "uCohesionWeight");
	glUniform1f(uniform_loc, _coheWeight);

	glDispatchCompute(ceil(numBoids / 256.0), 1, 1);

	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void ComputeShader::use() {
	
}
