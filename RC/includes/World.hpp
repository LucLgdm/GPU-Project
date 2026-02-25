/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   World.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/25 17:19:48 by lde-merc          #+#    #+#             */
/*   Updated: 2026/02/25 17:29:38 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// This class will represent the world, containing the map, the player, and other entities.
// It's pure logic, it should not contain any rendering code.

#pragma once

#include "Map.hpp"

struct Player {
    float posX;
    float posY;

    float dirX;
    float dirY;

    float planeX;
    float planeY;

    float moveSpeed;
    float rotSpeed;
};


class World {
	public:
		World();
		~World();

	private:
		Map _map;
		Player _player;

		// Other entities (enemies, items, etc.)
};