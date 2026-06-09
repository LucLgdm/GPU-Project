/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Compute.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/26 11:20:05 by lde-merc          #+#    #+#             */
/*   Updated: 2026/06/05 09:42:49 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Compute.hpp"

Compute::Compute(int height, int width): _height(height), _width(width) {
	cudaMalloc(&_accumBuffer, height * width * sizeof(float3));
	std::cout << "\033[32m[Compute]\033[0m \033[33mInitialized with resolution " << width << "x" << height << "\033[0m\n";
}

Compute::~Compute() {
	if (_accumBuffer) {
		cudaFree(_accumBuffer);
		_accumBuffer = nullptr;
	}
}

