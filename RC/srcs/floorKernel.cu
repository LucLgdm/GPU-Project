/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   floorKernel.cu             	                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/10 12:59:15 by lde-merc          #+#    #+#             */
/*   Updated: 2026/03/10 12:55:14 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Raycaster.hpp"

__global__ void floorKernel(uchar4 *pbo, Camera cam, uchar4* texFloor, uchar4* texCeil,
							int texW, int texH, int screenWidth, int screenHeight) {
	int x = blockIdx.x * blockDim.x + threadIdx.x;
	int y = blockIdx.y * blockDim.y + threadIdx.y;

	if (x >= screenWidth || y >= screenHeight)
		return;

	float rayDirX0 = cam.dirX - cam.planeX;
	float rayDirY0 = cam.dirY - cam.planeY;
	float rayDirX1 = cam.dirX + cam.planeX;
	float rayDirY1 = cam.dirY + cam.planeY;

	if (y == screenHeight / 2) return; // éviter division par zéro

	if (y > screenHeight / 2) {
		// Plafond
		float rowDistance = (float)(screenHeight / 2) / (float)(y - screenHeight / 2);

		float floorStepX = rowDistance * (rayDirX1 - rayDirX0) / screenWidth;
		float floorStepY = rowDistance * (rayDirY1 - rayDirY0) / screenWidth;

		float floorX = cam.x + rowDistance * rayDirX0 + floorStepX * x;
		float floorY = cam.y + rowDistance * rayDirY0 + floorStepY * x;

		int texX = ((int)(floorX * texW)) % texW;
		int texY = ((int)(floorY * texH)) % texH;
		if (texX < 0) texX += texW;
		if (texY < 0) texY += texH;

		float brightness = 0.4f;
		uchar4 t = texCeil[texY * texW + texX];
		pbo[y * screenWidth + x] = make_uchar4(t.x * brightness, t.y * brightness, t.z * brightness, 255);

	} else {
		// Sol
		float rowDistance = (float)(screenHeight / 2) / (float)(screenHeight / 2 - y);

		float floorStepX = rowDistance * (rayDirX1 - rayDirX0) / screenWidth;
		float floorStepY = rowDistance * (rayDirY1 - rayDirY0) / screenWidth;

		float floorX = cam.x + rowDistance * rayDirX0 + floorStepX * x;
		float floorY = cam.y + rowDistance * rayDirY0 + floorStepY * x;

		int texX = ((int)(floorX * texW)) % texW;
		int texY = ((int)(floorY * texH)) % texH;
		if (texX < 0) texX += texW;
		if (texY < 0) texY += texH;

		float brightness = 0.4f;
		uchar4 t = texFloor[texY * texW + texX];
		pbo[y * screenWidth + x] = make_uchar4(t.x * brightness, t.y * brightness, t.z * brightness, 255);
	}
}