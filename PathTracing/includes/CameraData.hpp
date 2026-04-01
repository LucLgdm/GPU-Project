/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CameraData.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/01 14:47:37 by lde-merc          #+#    #+#             */
/*   Updated: 2026/04/01 15:32:45 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

struct CameraData {
	float origin[3];
	float lowerLeftCorner[3];
	float horizontal[3];
	float vertical[3];
};