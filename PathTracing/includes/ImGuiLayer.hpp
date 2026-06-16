/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ImGuiLayer.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 12:54:16 by lde-merc          #+#    #+#             */
/*   Updated: 2026/06/16 18:52:12 by lde-merc         ###   ########.fr       */
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
#include "ImGui/imgui_spectrum.h"

#include <iostream>

#include "Camera.hpp"
#include "Scene.hpp"

class ImGuiLayer {
	public:
		ImGuiLayer();
		~ImGuiLayer();

		void init(GLFWwindow* window, int width, int height);
		void beginFrame();
		void render(Scene* scene, Camera* camera);
		void endFrame();
		void shutdown();

		void sceneLoader(Scene* scene);
		
		// ---Object---
		int displayListObject(Scene* scene);
		
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
	
		float _openglwidth;
		float _openglheight;
		float _viewPortWidth;
		float _viewPortHeight;
		
		std::string _errorMessage;
		bool _showErrorPopup = false;
		
		// Lights
		bool _uiAddLight = false;
		bool _uiSettingsLight = false;

		int _selectedDirLight = -1;
		int _selectedSpotLight = -1;
};
