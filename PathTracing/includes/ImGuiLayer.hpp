/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ImGuiLayer.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 12:54:16 by lde-merc          #+#    #+#             */
/*   Updated: 2026/06/12 16:25:47 by lde-merc         ###   ########.fr       */
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
		
		// ---Object---
		void displayListObject(Scene* scene);
		
		// ---Lights---
		void displayLight(Scene* scene);
		void addLight(Scene* scene);
		void settingsDirLight(Scene* scene, int& selectedLight);
		void settingsSpotLight(Scene* scene, int& selectedLight);
		
		// ---Error---
		void renderError();
		
	private:
		std::string _errorMessage;
		bool _showErrorPopup = false;
		
		// Lights
		bool _uiAddLight = false;
		bool _uiSettingsLight = false;
};
