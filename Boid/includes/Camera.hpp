/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Camera.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/20 12:12:12 by lde-merc          #+#    #+#             */
/*   Updated: 2026/03/20 16:35:29 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"


class Camera {
	public:
		Camera();
		~Camera();

		glm::mat4 getView();
		glm::mat4 getProjection(float);

		void updateProjectionMatrix(int, int);
	private:
		glm::vec3 _eye;		// Position
		glm::vec3 _target;	// Point looked
		glm::vec3 _up;		// Verticale
		glm::mat4 _projectionMatrix;
};