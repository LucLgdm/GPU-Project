/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Compute.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/26 11:17:59 by lde-merc          #+#    #+#             */
/*   Updated: 2026/04/14 13:34:26 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Scene.hpp"

struct uchar4;

class Compute {
	public:
		Compute(int, int);
		~Compute();
		
		void update(uchar4*, const SceneData&);
		void resetAccumulation();
	private:
		int _height;
		int _width;
		int _frameIndex = 0;
		float3* _accumBuffer = nullptr;
};