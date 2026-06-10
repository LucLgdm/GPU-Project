/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ImGuiLayer.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 12:54:16 by lde-merc          #+#    #+#             */
/*   Updated: 2026/06/10 16:22:44 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <ImGui/imgui_impl_glfw.h>
#include <ImGui/imgui_impl_opengl3.h>

#include "ImGui/ImGuiFileDialog.h"
#include <iostream>

#include "Scene.hpp"

class ImGuiLayer {
	public:
		ImGuiLayer();
		~ImGuiLayer();

		void init(GLFWwindow* window);
		void beginFrame();
		void render(Scene* scene);
		void endFrame();
		void shutdown();

		void sceneLoader(Scene* scene);
		void displayListObject(Scene* scene);
		void renderError();
		
	private:
		std::string _errorMessage;
		bool _showErrorPopup = false;
};
