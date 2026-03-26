/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Compute.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/26 11:17:59 by lde-merc          #+#    #+#             */
/*   Updated: 2026/03/26 11:28:37 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

struct uchar4;

class Compute {
	public:
		Compute(int, int);
		~Compute();
		
		void update(uchar4*);

	private:
		int _height;
		int _width;
};