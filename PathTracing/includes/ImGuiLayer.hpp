/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ImGuiLayer.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 12:54:16 by lde-merc          #+#    #+#             */
/*   Updated: 2026/06/15 16:22:01 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>
#include <ImGui/imgui_impl_glfw.h>
#include <ImGui/imgui_impl_opengl3.h>

#include "ImGui/ImGuiFileDialog.h"

#include "ImGuizmo.h"

#include <iostream>

#include "Camera.hpp"
#include "Scene.hpp"

class ImGuiLayer {
	public:
		ImGuiLayer();
		~ImGuiLayer();

		void init(GLFWwindow* window);
		void beginFrame();
		void render(Scene* scene, Camera* camera);
		void endFrame();
		void shutdown();

		void sceneLoader(Scene* scene);
		
		// ---Object---
		void displayListObject(Scene* scene);
		
		// ---Lights---
		void displayLight(Scene* scene);
		void addLight(Scene* scene);
		void settingsDirLight(Scene* scene);
		void settingsSpotLight(Scene* scene);
		
		// ---Error---
		void renderError();

		// ---ImGuizmo---
		void renderSpotlightGuizmo(Scene* scene, Camera* camera);
		
	private:
		std::string _errorMessage;
		bool _showErrorPopup = false;
		
		// Lights
		bool _uiAddLight = false;
		bool _uiSettingsLight = false;

		int _selectedDirLight = -1;
		int _selectedSpotLight = -1;
};
