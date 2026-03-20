/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Camera.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/20 12:12:12 by lde-merc          #+#    #+#             */
/*   Updated: 2026/03/20 19:38:47 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include <map>

struct GLFWwindow;

struct KeyState {
    bool isDown = false;
    bool wasDown = false;

    bool pressed() const {
        return isDown && !wasDown;
    }

    bool released() const {
        return !isDown && wasDown;
    }

    void update(bool currentDown) {
        wasDown = isDown;
        isDown = currentDown;
    }
};

class Camera {
	public:
		Camera();
		~Camera();
		void init(GLFWwindow *, int, int);

		glm::mat4 getView();
		glm::mat4 getProjection(float);

		void updateProjectionMatrix(int, int);
		void update(GLFWwindow *);
		
		void resetMouse(float, float);
		void beginRotate();
		void endRotate();
		void processMouseMove(float, float);
		
	private:
		glm::vec3 _eye;		// Position
		glm::vec3 _target;	// Point looked
		glm::vec3 _up;		// Verticale
		glm::mat4 _projectionMatrix;

		// Mouse
		int _lastX, _lastY;
		bool _rotating = false, _firstMouse = false;
		// Keys
		std::map<int, KeyState> _keys;
		// Move
		float _moveSpeed = 0.05f;
		float _yaw = -90.0f, _pitch = 0.0f;
		float _sensitivity = 0.2f;
};