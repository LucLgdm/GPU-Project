/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Raycaster.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/04 16:21:47 by lde-merc          #+#    #+#             */
/*   Updated: 2026/03/05 17:35:29 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cuda_runtime.h>
#include "Raycaster.hpp"

Raycaster::Raycaster(const std::string &mapFile, uchar4 *devPtr) : _devPtr(devPtr) {
	loadMap(mapFile);
	checkMapValidity();
}

Raycaster::~Raycaster() { }

void Raycaster::loadMap(const std::string &mapFile) {
	std::ifstream file(mapFile);
	if (!file.is_open())
		throw input_error("Failed to open map file: " + mapFile);
	
	std::string line;
	while (std::getline(file, line)) {
		if (line.substr(0, 5) == "#name ") {
			_mapName = line.substr(6);
			continue;
		}else if (line.substr(0, 3) == "#map" || line[0] == '\n') {
			continue;
		}else {
			std::vector<char> row(line.begin(), line.end());
			_map.push_back(row);
			_mapWidth = std::max(_mapWidth, static_cast<int>(row.size()));
		}
		
	}
	_mapHeight = static_cast<int>(_map.size());
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

void Raycaster::checkChar(const std::vector<char> &line) {
	for (char c : line) {
		if (c != '0' && c != '1' && c != 'E' && c != 'S' && c != 'W' && c != 'N' && c != ' ')
			throw input_error("Invalid character in map: " + std::string(1, c));
		if (c == 'E' || c == 'S' || c == 'W' || c == 'N') {
			setCameraDirection(c);
		}
	}
}

void Raycaster::floodFill(int x, int y) {
	if (x <= 0 || x >= _map[y].size() - 1 || y <= 0 || y >= _mapHeight - 1)
		throw input_error("Map is not closed");
	if (_map[y][x] == '1' || _map[y][x] == ' ' || _map[y][x] == 'V')
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
		throw input_error("Map is empty");
	for (const auto &row : _map)
		checkChar(row);	
	checkMapClosed();
}

void Raycaster::update() {
	// This function will be called every frame to update the raycasting logic.
	// It will launch the CUDA kernel to perform the raycasting calculations on the GPU.
	
}

uchar4* Renderer::mapPBO() {
	uchar4* devPtr = nullptr;
	size_t size = 0;
	cudaGraphicsMapResources(1, &_cudaPBO, 0);
	cudaGraphicsResourceGetMappedPointer((void**)&devPtr, &size, _cudaPBO);
	return devPtr;
}

void Renderer::unmapPBO() {
	cudaGraphicsUnmapResources(1, &_cudaPBO, 0);
}
