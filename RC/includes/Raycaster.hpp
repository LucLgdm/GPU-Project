/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Raycaster.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/25 17:37:26 by lde-merc          #+#    #+#             */
/*   Updated: 2026/03/17 11:40:05 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// This class will handle the raycasting logic, using CUDA to perform the calculations on the GPU.

#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <cmath>
#include "Exception.hpp"

struct Camera {
	float x, y; 			// Position
	float dirX, dirY; 		// Direction vector
	float planeX, planeY; 	// Camera plane (for field of view)
};

struct uchar4;
struct GLFWwindow;

class Raycaster {
	public:
		Raycaster(const std::string&, int, int);
		~Raycaster();

		void update(uchar4*);
		void updateFullscreen(int width, int height) { _screenWidth = width; _screenHeight = height; };
		void move(GLFWwindow*);

	private:
		std::string _mapName;
		std::vector<std::vector<char>> _map;
		std::vector<char> _flatMap; // Flattened map for CUDA
		int _mapWidth = 0;
		int _mapHeight = 0;
		int _screenWidth = 0;
		int _screenHeight = 0;
		
		int _texWidth = 0;
		int _texHeight = 0;

		char *_devMap = nullptr; // Device pointer for map data
		uchar4 *_devTexNS = nullptr; // Device pointer for texture data
		uchar4 *_devTexEW = nullptr; // Device pointer for texture data
		uchar4 *_devTexFloor = nullptr; // Device pointer for texture data
		uchar4 *_devTexCeil = nullptr; // Device pointer for texture data
		
		Camera _camera;
		
		void loadMap(const std::string&);
		void checkMapValidity();
		void setCameraDirection(char);
		void checkChar(const std::vector<char> &);
		void floodFill(int, int);
		void checkMapClosed();

		void loadTexture(const char*, uchar4**, int&, int&);

		void sendMapGpu();
};