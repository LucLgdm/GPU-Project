/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Raycaster.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/04 16:21:47 by lde-merc          #+#    #+#             */
/*   Updated: 2026/03/11 13:10:08 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <GLFW/glfw3.h>
#include "Raycaster.hpp"

/********************************************************************************
 * Init and map loading
 ********************************************************************************/

Raycaster::Raycaster(const std::string &mapFile, int screenWidth, int screenHeight) :
						_screenWidth(screenWidth), _screenHeight(screenHeight) {
	loadMap(mapFile);
	checkMapValidity();
	std::cout << "		\033[32mMap loaded successfully!\033[0m" << std::endl;

	loadTexture("ressources/textures/sandstone.jpg", &_devTexNS, _texWidth, _texHeight);
	loadTexture("ressources/textures/castle_brick.jpg", &_devTexEW, _texWidth, _texHeight);
	loadTexture("ressources/textures/rocky_floor.jpg", &_devTexFloor, _texWidth, _texHeight);
	std::cout << "		\033[32mTextures loaded successfully!\033[0m" << std::endl;
	
	sendMapGpu();
}
						
Raycaster::~Raycaster() { }

void Raycaster::loadMap(const std::string &mapFile) {
	std::ifstream file(mapFile);
	if (!file.is_open())
		throw inputError("	Failed to open map file: " + mapFile);
	
	std::string line;
	while (std::getline(file, line)) {
		if (line.substr(0, 6) == "#name ") {
			_mapName = line.substr(7);
		} else if (line.empty() || line[0] == '#') {
			continue;
		} else {
			std::vector<char> row(line.begin(), line.end());
			_map.push_back(row);
			_mapWidth = std::max(_mapWidth, static_cast<int>(row.size()));
		}
	}
	_mapHeight = static_cast<int>(_map.size());

	for (auto &line : _map)
		line.resize(_mapWidth, ' ');
}

void Raycaster::setCameraDirection(char c) {
	switch (c) {
		case 'N':
			_camera.dirX = 0;
			_camera.dirY = -1;
			_camera.planeX = 0.66f;
			_camera.planeY = 0;
			break;
		case 'S':
			_camera.dirX = 0;
			_camera.dirY = 1;
			_camera.planeX = -0.66f;
			_camera.planeY = 0;
			break;
		case 'E':
			_camera.dirX = 1;
			_camera.dirY = 0;
			_camera.planeX = 0;
			_camera.planeY = 0.66f;
			break;
		case 'W':
			_camera.dirX = -1;
			_camera.dirY = 0;
			_camera.planeX = 0;
			_camera.planeY = -0.66f;
			break;
	}
}

static bool cameraSet = false;

/********************************************************************
 * Map validation
 * ******************************************************************/

void Raycaster::checkChar(const std::vector<char> &line) {
	for (char c : line) {
		if (c != '0' && c != '1' && c != 'E' && c != 'S' && c != 'W' && c != 'N' && c != ' ')
			throw inputError("	Invalid character in map: " + std::string(1, c));
		if (c == 'E' || c == 'S' || c == 'W' || c == 'N') {
			if (cameraSet)
				throw inputError("	Multiple camera positions found in map");
			cameraSet = true;
			setCameraDirection(c);
		}
	}
}

void Raycaster::floodFill(int x, int y) {
	if (x < 0 || x >= _mapWidth || y < 0 || y >= _mapHeight)
		throw inputError("	Map is not closed : out of bounds at (" + std::to_string(x) + ", " + std::to_string(y) + ")");
	if (_map[y][x] == ' ')
		throw inputError("	Map is not closed : space found at (" + std::to_string(x) + ", " + std::to_string(y) + ")");
	if (_map[y][x] == '1' || _map[y][x] == 'V')
		return;
	_map[y][x] = 'V';
	floodFill(x + 1, y);
	floodFill(x - 1, y);
	floodFill(x, y + 1);
	floodFill(x, y - 1);
}

void Raycaster::checkMapClosed() {
	for (int y = 0; y < _mapHeight; y++) {
		for (int x = 0; x < _map[y].size(); x++) {
			if (_map[y][x] == 'E' || _map[y][x] == 'S' || _map[y][x] == 'W' || _map[y][x] == 'N') {
				_camera.x = x; _camera.y = y;
			}
		}
	}
	floodFill(_camera.x, _camera.y);

	for (auto &row : _map) {
		for (int i = 0; i < row.size(); i++) {
			if (row[i] == 'V')
				row[i] = '0';
		}
	}
}

void Raycaster::checkMapValidity() {
	if (_map.empty())
		throw inputError("	Map is empty");
	for (const auto &row : _map)
		checkChar(row);
	if (!cameraSet)
		throw inputError("	No camera position found in map");
	checkMapClosed();
}

/**************************************************************************************
 * Camera movement
 **************************************************************************************/
 
void Raycaster::move(GLFWwindow* window) {
	float moveSpeed = 0.01f;
	const float rotSpeed = 0.01f;
	const float offset = 0.2f; // to avoid collision issues when close to walls

	// Run
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		moveSpeed *= 2;

	// deplacements
	auto tryMove = [&](float newX, float newY) {
		float origX = _camera.x;
		float origY = _camera.y;
		
		if (_map[static_cast<int>(origY)][static_cast<int>(newX + offset)] != '1' &&
			_map[static_cast<int>(origY)][static_cast<int>(newX - offset)] != '1')
			_camera.x = newX;
		if (_map[static_cast<int>(newY + offset)][static_cast<int>(origX)] != '1' &&
			_map[static_cast<int>(newY - offset)][static_cast<int>(origX)] != '1')
			_camera.y = newY;
	};

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		tryMove(_camera.x + _camera.dirX * moveSpeed, _camera.y + _camera.dirY * moveSpeed);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		tryMove(_camera.x - _camera.dirX * moveSpeed, _camera.y - _camera.dirY * moveSpeed);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		tryMove(_camera.x - _camera.planeX * moveSpeed, _camera.y - _camera.planeY * moveSpeed);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		tryMove(_camera.x + _camera.planeX * moveSpeed, _camera.y + _camera.planeY * moveSpeed);

	// Rotation
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		float oldDirX = _camera.dirX;
		_camera.dirX = _camera.dirX * cos(rotSpeed) - _camera.dirY * sin(rotSpeed);
		_camera.dirY = oldDirX * sin(rotSpeed) + _camera.dirY * cos(rotSpeed);
		float oldPlaneX = _camera.planeX;
		_camera.planeX = _camera.planeX * cos(rotSpeed) - _camera.planeY * sin(rotSpeed);
		_camera.planeY = oldPlaneX * sin(rotSpeed) + _camera.planeY * cos(rotSpeed);
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		float oldDirX = _camera.dirX;
		_camera.dirX = _camera.dirX * cos(-rotSpeed) - _camera.dirY * sin(-rotSpeed);
		_camera.dirY = oldDirX * sin(-rotSpeed) + _camera.dirY * cos(-rotSpeed);
		float oldPlaneX = _camera.planeX;
		_camera.planeX = _camera.planeX * cos(-rotSpeed) - _camera.planeY * sin(-rotSpeed);
		_camera.planeY = oldPlaneX * sin(-rotSpeed) + _camera.planeY * cos(-rotSpeed);
	}

}
