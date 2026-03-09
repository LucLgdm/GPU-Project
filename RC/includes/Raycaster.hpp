/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Raycaster.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/25 17:37:26 by lde-merc          #+#    #+#             */
/*   Updated: 2026/03/09 18:27:01 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// This class will handle the raycasting logic, using CUDA to perform the calculations on the GPU.

#pragma once

#include <vector>
#include <string>
#include <fstream>
#include "Exception.hpp"

struct Camera {
	float x, y; // Position
	float dirX, dirY; // Direction vector
	float planeX, planeY; // Camera plane (for field of view)
};

struct uchar4;

class Raycaster {
	public:
		Raycaster(const std::string&);
		~Raycaster();

		
		void update(uchar4*);
	private:
		std::string _mapName;
		std::vector<std::vector<char>> _map;
		std::vector<int> _flatMap; // Flattened map for CUDA
		int _mapWidth = 0;
		int _mapHeight = 0;
				
		Camera _camera;
		
		void loadMap(const std::string&);
		void checkMapValidity();
		void setCameraDirection(char);
		void checkChar(const std::vector<char> &);
		void floodFill(int, int);
		void checkMapClosed();

		void sendMapGpu();
};