/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ImGuiLayer.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 12:54:16 by lde-merc          #+#    #+#             */
/*   Updated: 2026/06/04 13:07:53 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <ImGui/imgui_impl_glfw.h>
#include <ImGui/imgui_impl_opengl3.h>



class ImGuiLayer {
	public:
		ImGuiLayer();
		~ImGuiLayer();

		void init(GLFWwindow* window);
		void beginFrame();
		void render();
		void endFrame();
		void shutdown();
		
};
