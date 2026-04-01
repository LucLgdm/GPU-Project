/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Camera.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/25 18:11:11 by lde-merc          #+#    #+#             */
/*   Updated: 2026/04/01 15:32:29 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include <map>
#include "CameraData.hpp"

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
		const glm::vec3& getEyes() const { return _eye; };
		const glm::vec3& getTarget() const { return _target; };
		const glm::vec3& getUp() const { return _up; };
		const float& getFov() const {return _fov; };
		const float& getAspect() const {return _aspect; };

		void updateProjectionMatrix(int, int);
		void updatePos(GLFWwindow *);
		void updatePlan();
		
		void resetMouse(float, float);
		void beginRotate();
		void endRotate();
		void processMouseMove(float, float);
		
	private:
		glm::vec3 _eye;		// Position
		glm::vec3 _target;	// Point looked
		glm::vec3 _up;		// Verticale
		glm::mat4 _projectionMatrix;

		float _fov;
		float _aspect;
		// Mouse
		int _lastX, _lastY;
		bool _rotating = false, _firstMouse = false;
		// Keys
		std::map<int, KeyState> _keys;
		// Move
		float _moveSpeed = 0.03f;
		float _yaw = -90.0f, _pitch = 0.0f;
		float _sensitivity = 0.2f;
};