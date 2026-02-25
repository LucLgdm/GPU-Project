/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Map.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/25 17:05:01 by lde-merc          #+#    #+#             */
/*   Updated: 2026/02/25 17:39:41 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <vector>
#include <string>

class Map {
	public:
		Map();
		~Map();

	private:
		int _width;
		int _height;
		std::string _name;
		std::vector<int> _map;
};

