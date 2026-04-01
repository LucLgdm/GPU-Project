/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Camera.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/25 18:10:59 by lde-merc          #+#    #+#             */
/*   Updated: 2026/04/01 15:43:38 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Camera.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

void uploadCamera(const CameraData&);

Camera::Camera(): _eye(glm::vec3(0.0f, 0.0f, 10.0f)), _target(glm::vec3(0.0f, 0.0f, 0.0f)),
					_up(glm::vec3(0.0f, 1.0f, 0.0f)), _fov(60.0f) { }

Camera::~Camera() { }


void Camera::init(GLFWwindow *window, int width, int height) {
	_lastX = width / 2.0f;
	_lastY = height / 2.0f;
	glfwSetCursorPos(window, _lastX, _lastY);
	
	_aspect = (float)width / height;
	_projectionMatrix = glm::perspective(glm::radians(_fov / 2.0f), _aspect, 0.1f, 3000.0f);

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


void Camera::updatePos(GLFWwindow *window) {
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


void Camera::updatePlan() {
	glm::vec3 w = normalize(_eye - _target);
	glm::vec3 u = normalize(cross(_up, w));
	glm::vec3 v = cross(w, u);

	float halfH = tan(glm::radians(_fov * 0.5f));
	float halfW = _aspect * halfH;

	CameraData cam;

	cam.origin[0] = _eye.x;
	cam.origin[1] = _eye.y;
	cam.origin[2] = _eye.z;

	cam.lowerLeftCorner[0] = cam.origin[0] - halfW * u.x - halfH * v.x - w.x;
	cam.lowerLeftCorner[1] = cam.origin[1] - halfW * u.y - halfH * v.y - w.y;
	cam.lowerLeftCorner[2] = cam.origin[2] - halfW * u.z - halfH * v.z - w.z;
	
	cam.horizontal[0] = 2 * halfW * u.x;
	cam.horizontal[1] = 2 * halfW * u.y;
	cam.horizontal[2] = 2 * halfW * u.z;
	
	cam.vertical[0] = 2 * halfH * v.x;
	cam.vertical[1] = 2 * halfH * v.y;
	cam.vertical[2] = 2 * halfH * v.z;

	uploadCamera(cam);
}
