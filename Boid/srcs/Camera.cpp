/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Camera.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/20 12:11:43 by lde-merc          #+#    #+#             */
/*   Updated: 2026/03/20 17:09:40 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Camera.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

Camera::Camera(): _eye(glm::vec3(0.0f, 0.0f, 15.0f)),
					_target(glm::vec3(0.0f, 0.0f, 0.0f)),
					_up(glm::vec3(0.0f, 1.0f, 0.0f)),
					_projectionMatrix(glm::perspective(glm::radians(30.0f), 1600.0f/1200.0f, 0.1f, 3000.0f))
					{ }

Camera::~Camera() { }

glm::mat4 Camera::getView() {
	glm::vec3 forward = normalize(_target - _eye);	
	glm::vec3 right = normalize(cross(forward, _up));
	glm::vec3 up = normalize(cross(right, forward));

	return glm::lookAt(_eye, _target, up);
}

glm::mat4 Camera::getProjection(float aspectRatio) {
	return _projectionMatrix;
}

void Camera::updateProjectionMatrix(int width, int height) {
			float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
			_projectionMatrix = glm::perspective(glm::radians(30.0f), aspectRatio, 0.1f, 3000.0f);
}

void Camera::update(GLFWwindow *window) {
	// Mouse delta
	double mouseX, mouseY;
	glfwGetCursorPos(window, &mouseX, &mouseY);
	float deltaX = static_cast<float>(mouseX) - _lastX;
	float deltaY = _lastY - static_cast<float>(mouseY);
	_lastX = static_cast<float>(mouseX);
	_lastY = static_cast<float>(mouseY);

	_yaw   += deltaX * _sensitivity;
	_pitch += deltaY * _sensitivity;
	_pitch  = glm::clamp(_pitch, -89.0f, 89.0f);

	// Recalculate forward
	glm::vec3 forward;
	forward.x = cos(glm::radians(_yaw)) * cos(glm::radians(_pitch));
	forward.y = sin(glm::radians(_pitch));
	forward.z = sin(glm::radians(_yaw)) * cos(glm::radians(_pitch));
	forward   = normalize(forward);

	glm::vec3 right = normalize(cross(forward, glm::vec3(0, 1, 0)));

	// WASD
	for (auto &keyPair : _keys)
		keyPair.second.update(glfwGetKey(window, keyPair.first) == GLFW_PRESS);

	if (_keys[GLFW_KEY_W].isDown) _eye += forward * _moveSpeed;
	if (_keys[GLFW_KEY_S].isDown) _eye -= forward * _moveSpeed;
	if (_keys[GLFW_KEY_A].isDown) _eye -= right   * _moveSpeed;
	if (_keys[GLFW_KEY_D].isDown) _eye += right   * _moveSpeed;

	_target = _eye + forward;
}