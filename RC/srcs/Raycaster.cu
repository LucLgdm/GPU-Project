/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Raycaster.cu                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/04 16:21:47 by lde-merc          #+#    #+#             */
/*   Updated: 2026/03/10 14:12:29 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Raycaster.hpp"
#include <cuda_runtime.h>

__global__ void raycastKernel(uchar4*, Camera, char*, int, int, int, int);

Raycaster::Raycaster(const std::string &mapFile, int screenWidth, int screenHeight) :
						_screenWidth(screenWidth), _screenHeight(screenHeight) {
	loadMap(mapFile);
	checkMapValidity();
	std::cout << "		\033[32mMap loaded successfully!\033[0m" << std::endl;
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
	if (x < 0 || x >= _map[y].size() || y < 0 || y >= _mapHeight)
		throw inputError("	Map is not closed");
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
		throw inputError("	Map is empty");
	for (const auto &row : _map)
		checkChar(row);
	if (!cameraSet)
		throw inputError("	No camera position found in map");
	checkMapClosed();
}

void Raycaster::sendMapGpu() {
	_flatMap.reserve(_mapWidth * _mapHeight);
	for (const auto &row : _map) {
		for (char c : row)
			_flatMap.push_back(c);
		// Compléter jusqu'à _mapWidth
		for (int i = row.size(); i < _mapWidth; i++)
			_flatMap.push_back(' ');
	}

	cudaMalloc(&_devMap, _flatMap.size() * sizeof(char));
	cudaMemcpy(_devMap, _flatMap.data(), _flatMap.size() * sizeof(char), cudaMemcpyHostToDevice);
}


void Raycaster::update(uchar4 *devPtr) {
	// This function will be called every frame to update the raycasting logic.
	// It will launch the CUDA kernel to perform the raycasting calculations on the GPU.
	int nbThreads = _screenWidth;
	int blockSize = 256;
	int nbBlocks = (nbThreads + blockSize - 1) / blockSize;

	raycastKernel<<<nbBlocks, blockSize>>>(devPtr, _camera, _devMap,
												_mapWidth, _mapHeight,
												_screenWidth, _screenHeight);
	cudaDeviceSynchronize();
	
	cudaError_t err = cudaGetLastError();
	if (err != cudaSuccess)
		throw cuda_Error(cudaGetErrorString(err));
}
