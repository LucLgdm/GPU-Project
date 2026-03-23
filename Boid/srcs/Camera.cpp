/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Camera.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/20 12:11:43 by lde-merc          #+#    #+#             */
/*   Updated: 2026/03/23 09:46:53 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Camera.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

Camera::Camera(): _eye(glm::vec3(0.0f, 0.0f, 60.0f)),
					_target(glm::vec3(0.0f, 0.0f, 0.0f)),
					_up(glm::vec3(0.0f, 1.0f, 0.0f)),
					_projectionMatrix(glm::perspective(glm::radians(30.0f), 1600.0f/1200.0f, 0.1f, 3000.0f)) {}

Camera::~Camera() { }


void Camera::init(GLFWwindow *window, int width, int height) {
	_lastX = width / 2.0f;
	_lastY = height / 2.0f;
	glfwSetCursorPos(window, _lastX, _lastY);
}

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

void Camera::beginRotate() {
	_rotating = true;
	_firstMouse = true;
}

void Camera::endRotate() {
	_rotating = false;
}

void Camera::processMouseMove(float xpos, float ypos) {
	if (!_rotating)
		return;

	if (_firstMouse) {
		_lastX = xpos;
		_lastY = ypos;
		_firstMouse = false;
		return;
	}

	float dx = xpos - _lastX;
	float dy = _lastY - ypos;

	_lastX = xpos;
	_lastY = ypos;

	_yaw   += dx * _sensitivity;
	_pitch += dy * _sensitivity;

	_pitch = glm::clamp(_pitch, -89.0f, 89.0f);
}


void Camera::update(GLFWwindow *window) {
	// Recalculate forward
	glm::vec3 forward;
	forward.x = cos(glm::radians(_yaw)) * cos(glm::radians(_pitch));
	forward.y = sin(glm::radians(_pitch));
	forward.z = sin(glm::radians(_yaw)) * cos(glm::radians(_pitch));
	forward   = normalize(forward);

	glm::vec3 right = normalize(cross(forward, glm::vec3(0, 1, 0)));
	glm::vec3 up = normalize(cross(right, forward));

	// WASD
	for (auto &keyPair : _keys)
		keyPair.second.update(glfwGetKey(window, keyPair.first) == GLFW_PRESS);

	if (_keys[GLFW_KEY_W].isDown) _eye += forward * _moveSpeed;
	if (_keys[GLFW_KEY_S].isDown) _eye -= forward * _moveSpeed;
	if (_keys[GLFW_KEY_A].isDown) _eye -= right   * _moveSpeed;
	if (_keys[GLFW_KEY_D].isDown) _eye += right   * _moveSpeed;
	if (_keys[GLFW_KEY_Q].isDown) _eye +=  up * _moveSpeed;
	if (_keys[GLFW_KEY_E].isDown) _eye -=  up * _moveSpeed;

	_target = _eye + forward;
}

void Camera::resetMouse(float x, float y) {
	_lastX = x;
	_lastY = y;
}