/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Compute.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/26 11:20:05 by lde-merc          #+#    #+#             */
/*   Updated: 2026/04/14 13:34:51 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Compute.hpp"

Compute::Compute(int height, int width): _height(height), _width(width) {
	cudaMalloc(&_accumBuffer, height * width * sizeof(float3));
}

Compute::~Compute() {
	cudaFree(_accumBuffer);
}

