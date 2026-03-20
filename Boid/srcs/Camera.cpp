/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Camera.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/20 12:11:43 by lde-merc          #+#    #+#             */
/*   Updated: 2026/03/20 13:25:59 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Camera.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

Camera::Camera(): _eye(glm::vec3(1.0f, 0.0f, 3.0f)),
					_target(glm::vec3(0.0f, 0.0f, 0.0f)),
					_up(glm::vec3(0.0f, 1.0f, 0.0f)) { }

Camera::~Camera() { }

glm::mat4 Camera::getView() {
	glm::vec3 forward = normalize(_target - _eye);	
	glm::vec3 right = normalize(cross(forward, _up));
	glm::vec3 up = normalize(cross(right, forward));

	return glm::lookAt(_eye, _target, up);
}

glm::mat4 Camera::getProjection(float aspectRatio) {
	return glm::perspective(glm::radians(30.0f), aspectRatio, 0.1f, 3000.0f);
}
