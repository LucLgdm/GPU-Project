/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Raycaster.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/25 17:37:26 by lde-merc          #+#    #+#             */
/*   Updated: 2026/03/04 16:40:13 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// This class will handle the raycasting logic, using CUDA to perform the calculations on the GPU.

#pragma once

#include <vector>
#include <string>
#include "Exception.hpp"

struct Camera {
	float x, y; // Position
	float dirX, dirY; // Direction vector
	float planeX, planeY; // Camera plane (for field of view)
};

struct uchar4;

class Raycaster {
	public:
		Raycaster(const std::string&, uchar4*);
		~Raycaster();

		void update();
	private:
		std::vector<std::vector<int>> _map;
		int _mapWidth;
		int _mapHeight;
		uchar4* _devPtr;
		
		Camera _camera;
		
		void loadMap(const std::string&);
		void checkMapValidity();
};