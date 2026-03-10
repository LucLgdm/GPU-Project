/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Kernel.cu                   	                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/10 12:59:15 by lde-merc          #+#    #+#             */
/*   Updated: 2026/03/10 12:55:14 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Raycaster.hpp"

__global__ void raycastKernel(uchar4*, Camera, char*, int, int, int, int);

__global__ void raycastKernel(uchar4* pbo, Camera cam, char* map, 
							int mapWidth, int mapHeight,
							int screenWidth, int screenHeight)
{
	int idx = blockIdx.x * blockDim.x + threadIdx.x;
	if (idx >= screenWidth) return;

	// Calculate ray position and direction
	float cameraX = 2.0f * idx / screenWidth - 1.0f; // Coordinate in camera space : [-1, 1]
	float rayDirX = cam.dirX + cam.planeX * cameraX;
	float rayDirY = cam.dirY + cam.planeY * cameraX;
	
	// Map position
	int mapX = (int)cam.x;
	int mapY = (int)cam.y;

	// Length of ray from current position to next x or y-side
	float sideDistX;
	float sideDistY;

	// Length of ray from one x or y-side to next x or y-side
	// if rayDirX is 0, deltaDistX will be inf, so we handle that case
	float deltaDistX = fabsf(1.0f / rayDirX);
	float deltaDistY = fabsf(1.0f / rayDirY);
	float perpWallDist;

	// Direction to step in x and y
	int stepX;
	int stepY;
	int hit = 0; // Was there a wall hit?
	int side; // Was a NS or a EW wall hit?
	
	// Calculate step and initial sideDist
	if (rayDirX < 0) {
		stepX = -1;
		sideDistX = (cam.x - mapX) * deltaDistX;
	} else {
		stepX = 1;
		sideDistX = (mapX + 1.0f - cam.x) * deltaDistX;
	}
	if (rayDirY < 0) {
		stepY = -1;
		sideDistY = (cam.y - mapY) * deltaDistY;
	} else {
		stepY = 1;
		sideDistY = (mapY + 1.0f - cam.y) * deltaDistY;
	}

	// Perform DDA (Digital Differential Analyzer)
	while (hit == 0) {
		// Jump to next map square, either in x-direction, or in y-direction
		if (sideDistX < sideDistY) {
			sideDistX += deltaDistX;
			mapX += stepX;
			side = 0;
		} else {
			sideDistY += deltaDistY;
			mapY += stepY;
			side = 1;
		}
		// Check if ray has hit a wall
		if (mapY >= 0 && mapY < mapHeight && mapX >= 0 && mapX < mapWidth
			&& map[mapY * mapWidth + mapX] == '1') hit = 1;
	}

	// Calculate distance projected on camera direction (Euclidean distance will give fisheye effect!)
	// The last addition of deltaDist went one step too far, so we remove it
	if (side == 0)
		perpWallDist = sideDistX - deltaDistX;
	else
		perpWallDist = sideDistY - deltaDistY;

	int lineHeight = (int)(screenHeight / perpWallDist);

	int drawStart = screenHeight / 2 - lineHeight / 2;
	int drawEnd   = screenHeight / 2 + lineHeight / 2;

	// Clamp values to screen boundaries
	if (drawStart < 0) drawStart = 0;
	if (drawEnd >= screenHeight) drawEnd = screenHeight - 1;

	for (int y = 0; y < screenHeight; y++) {
		uchar4 color;
		if (y < drawStart) {
			color = make_uchar4(50, 50, 50, 255);   // plafond
		} else if (y <= drawEnd) {
			if (side == 1)
				color = make_uchar4(255, 0, 0, 255); // mur rouge
			else
				color = make_uchar4(0, 0, 255, 255); // mur bleu
		} else {
			color = make_uchar4(100, 100, 100, 255); // sol
		}
		pbo[y * screenWidth + idx] = color;
	}
}