/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ImGuiLayer.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 12:56:58 by lde-merc          #+#    #+#             */
/*   Updated: 2026/06/12 17:17:55 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ImGuiLayer.hpp"

ImGuiLayer::ImGuiLayer() {}

ImGuiLayer::~ImGuiLayer() {}

/************************************************************************
 * Necessary
 * **********************************************************************/
void ImGuiLayer::init(GLFWwindow* window) {
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui::StyleColorsDark();
	ImGui_ImplOpenGL3_Init("#version 330");
	
	std::cout << "\033[32m[ImGuiLayer]\033[0m \033[33mImGui layer initialized.\033[0m" << std::endl;
}

void ImGuiLayer::beginFrame() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void ImGuiLayer::endFrame() {
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiLayer::shutdown() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

/************************************************************************
 * Render
 * **********************************************************************/
void ImGuiLayer::render(Scene* scene) {
	ImGui::Begin("Hello ImGui");
	
	sceneLoader(scene);
	displayListObject(scene);
	displayLight(scene);
	if (_uiAddLight)
		addLight(scene);
		
	ImGui::End();

	if (_showErrorPopup) {
		ImGui::OpenPopup("Error");
		_showErrorPopup = false;
	}
	renderError();
}

/************************************************************************
 * Object
 * **********************************************************************/
void ImGuiLayer::sceneLoader(Scene* scene) {
	if (ImGui::Button("Load Object")) {
		IGFD::FileDialogConfig config;
		config.path = ".";
		ImGuiFileDialog::Instance()->OpenDialog("OpenSceneDlg", "Open Object", ".obj", config);
	}

	if (ImGuiFileDialog::Instance()->Display("OpenSceneDlg")) {
		if (ImGuiFileDialog::Instance()->IsOk()) {
			std::string filePath = ImGuiFileDialog::Instance()->GetFilePathName();
			std::string fileName = ImGuiFileDialog::Instance()->GetCurrentFileName();

			std::cout << "Selected file: " << filePath << std::endl;
			std::cout << "File name : " << fileName << std::endl;
			try {
				scene->addObject(filePath, fileName);
			} catch (std::runtime_error& e) {				
				_errorMessage = e.what();
				_showErrorPopup = true;
			}
		}

		ImGuiFileDialog::Instance()->Close();
	}
}

void ImGuiLayer::displayListObject(Scene* scene) {
	static int selectedObject = -1;
	if (ImGui::BeginTable("SceneObjects", 2)) {
		for (int i = 0; i < scene->getObjects().size(); ++i) {
			ImGui::TableNextRow();

			ImGui::TableSetColumnIndex(0);

			bool selected = (selectedObject == i);

			if (ImGui::Selectable(scene->getObjName()[i].c_str(), selected))
				selectedObject = i;

			ImGui::TableSetColumnIndex(1);

			if (ImGui::Button(("Delete##" + std::to_string(i)).c_str())) {
				scene->removeObject(i);
				selectedObject = -1;
				break;
			}
		}
		ImGui::EndTable();
	}

	if (selectedObject >= 0) {
		ImGui::Begin("Properties");
		
		auto& object = scene->getObjects()[selectedObject];

		ImGui::Separator();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Object Properties");

		size_t count = object.getTriangleCount();
		ImGui::Text("Name: %s", scene->getObjName()[selectedObject].c_str());
		ImGui::Text("Triangles: %llu", static_cast<long long int>(count));

		if (ImGui::Button("OK"))
			selectedObject = -1;
		ImGui::End();
	}
}

/************************************************************************
 * Lights
 * **********************************************************************/
void ImGuiLayer::displayLight(Scene* scene) {
	if (ImGui::Button("Add a Light")) _uiAddLight = true;

	static int selectedDirLight = -1;
	static int selectedSpotLight = -1;
	
	if (ImGui::BeginTable("SceneLight", 1)) {
		for (int i = 0; i < scene->getDirLights().size(); ++i) {
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			bool selectedDir = (selectedDirLight == i);
			std::string label = "Dir Light " + std::to_string(scene->getDirLight(i).index);
			if (ImGui::Selectable(label.c_str(), selectedDir))
				selectedDirLight = i;
		}

		for (int i = 0; i < scene->getSpotLights().size(); ++i) {
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			bool selectedSpot = (selectedSpotLight == i);
			std::string label = "Spot Light " + std::to_string(scene->getSpotLight(i).index);
			if (ImGui::Selectable(label.c_str(), selectedSpot))
				selectedSpotLight = i;
		}
		ImGui::EndTable();
	}
	
	if (selectedDirLight >= 0)
		settingsDirLight(scene, selectedDirLight);
	if (selectedSpotLight >= 0)
		settingsSpotLight(scene, selectedSpotLight);
}

void ImGuiLayer::addLight(Scene* scene) {
	static float color[3] = {1.0f, 1.0f, 1.0f};
	static float direction[3] = {0.0f, -1.0f, 0.0f};
	static float intensity = 1.0f;
	static float angle[2] = {0.0f, 0.0f};
	static float position[3] = {0.0f, 0.0f, 0.0f};
	
	ImGui::Begin("Add a Light");
		static int uiType = -1;
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Type");
		ImGui::RadioButton("Directionnal", &uiType, 0); ImGui::SameLine();
		ImGui::RadioButton("SpotLight", &uiType, 1); // ImGui::SameLine();
		
		switch (uiType) {
			case 0: {
				ImGui::ColorPicker3("Color", color); ImGui::Separator();
				ImGui::DragFloat3("Direction", direction, 0.01f); ImGui::Separator();
				ImGui::SliderFloat("Intensity", &intensity, 0.0f, 2.0f);
				
				break ;
			} case 1: {
				ImGui::DragFloat3("Position", position, 0.01f);
				ImGui::DragFloat3("Direction", direction, 0.01f); ImGui::Separator();
				ImGui::ColorPicker3("Color", color); ImGui::Separator();
				ImGui::SliderFloat("Intensity", &intensity, 0.0f, 2.0f); ImGui::Separator();
				ImGui::DragFloat2("InnerCutoff && OuterCutOff", angle, 1.0f, 0.0f, 45.0f);
				break ;
			}
		}
		if (ImGui::Button("Add")) {
			switch (uiType) {
				case 0: {
					DirLight dirL(make_float3(direction[0], direction[1], direction[2]),
								  make_float3(color[0], color[1], color[2]),
								  intensity);
					scene->addDirLight(dirL);
					break ;
				} case 1: {
					SpotLight spotL(make_float3(position[0], position[1], position[2]),
									make_float3(direction[0], direction[1], direction[2]),
									make_float3(color[0], color[1], color[2]),
									intensity, angle[0], angle[1]);
					scene->addSpotLight(spotL);
					break ;
				}
			}
			_uiAddLight = false;
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel"))
			_uiAddLight = false;
			
		ImGui::End();
}

	// ---Directionnal Light---
void ImGuiLayer::settingsDirLight(Scene* scene, int& selectedDirLight) {
	ImGui::Begin("Properties");

	auto& lightDir = scene->getDirLight(selectedDirLight);

		// ---Color---
	static float colorPro[3]; lightDir.getColor(colorPro);
	float tmp[3]; lightDir.getColor(tmp);

	ImGui::ColorPicker3("Color", colorPro);
	bool changed = (colorPro[0] != tmp[0]) || (colorPro[1] != tmp[1]) || (colorPro[2] != tmp[2]);
	if (changed) {
		lightDir.setColor(colorPro);
		scene->setUpdated(true);
		scene->setDirLightDirty(true);
	}
	
		// ---Direction---
	static float yaw = 0.0f;
	static float pitch = -45.0f;
	float tmpDir[3];
	lightDir.getDirection(tmp);
	
	ImGui::SliderFloat("Yaw", &yaw, -180.0f, 180.0f);
	ImGui::SliderFloat("Pitch", &pitch, -89.0f, 89.0f);
	lightDir.setDirection(yaw, pitch);
	
	lightDir.getDirection(tmpDir);
	changed = (tmp[0] != tmpDir[0] || tmp[1] != tmpDir[1] || tmp[2] != tmpDir[2]);
	if (changed) {
		scene->setUpdated(true);
		scene->setDirLightDirty(true);
	}
	
		// ---Intensity---
	static float intensityPro = lightDir.intensity;
	ImGui::SliderFloat("Intensity", &intensityPro, 0.0f, 2.0f);
	if (intensityPro != lightDir.intensity) {
		lightDir.setIntensity(intensityPro);
		scene->setUpdated(true);
		scene->setDirLightDirty(true);
	}
	
	if (ImGui::Button("OK"))
		selectedDirLight = -1;
	ImGui::End();
}
	// ---SpotLight---
void ImGuiLayer::settingsSpotLight(Scene* scene, int& selectedLight) {
	ImGui::Begin("Properties");
	auto& spotLight = scene->getSpotLight(selectedLight);
	
		// ---Color---
	static float colorSpot[3]; spotLight.getColor(colorSpot);
	float tmp[3]; spotLight.getColor(tmp);
	
	ImGui::ColorPicker3("Color", colorSpot);
	bool changed = (colorSpot[0] != tmp[0] || colorSpot[1] != tmp[1] || colorSpot[2] != tmp[2]);
	if (changed) {
		spotLight.setColor(colorSpot);
		scene->setUpdated(true);
		scene->setSpotLightDirty(true);
	}
	
	if (ImGui::Button("Ok"))
		selectedLight = -1;
	ImGui::End();
}

/************************************************************************
 * Error
 * **********************************************************************/
void ImGuiLayer::renderError() {
	if (ImGui::BeginPopupModal("Error", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), _errorMessage.c_str());

		ImGui::Separator();

		if (ImGui::Button("OK"))
			ImGui::CloseCurrentPopup();

		ImGui::EndPopup();
	}
}

