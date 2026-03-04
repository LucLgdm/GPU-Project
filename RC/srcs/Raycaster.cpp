/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Raycaster.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/04 16:21:47 by lde-merc          #+#    #+#             */
/*   Updated: 2026/03/04 16:41:58 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cuda_runtime.h>
#include "Raycaster.hpp"

Raycaster::Raycaster(const std::string &mapFile, uchar4 *devPtr) : _devPtr(devPtr) {
	loadMap(mapFile);
	checkMapValidity();
}

Raycaster::~Raycaster() { }

void Raycaster::update() {
	// This function will be called every frame to update the raycasting logic.
	// It will launch the CUDA kernel to perform the raycasting calculations on the GPU.
	
}