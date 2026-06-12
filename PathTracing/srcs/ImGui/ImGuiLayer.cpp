/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ImGuiLayer.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 12:56:58 by lde-merc          #+#    #+#             */
/*   Updated: 2026/06/11 18:30:28 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ImGuiLayer.hpp"

ImGuiLayer::ImGuiLayer() {}

ImGuiLayer::~ImGuiLayer() {}

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
	if (_uiAddLight) addLight(scene);
	if (_uiSettingsLight) settingsLight(scene);
	ImGui::End();

	if (_showErrorPopup) {
		ImGui::OpenPopup("Error");
		_showErrorPopup = false;
	}
	renderError();
}

	// ---Object---
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

	// ---Light---
void ImGuiLayer::displayLight(Scene* scene) {
	if (ImGui::Button("Add a Light")) _uiAddLight = true;
	ImGui::SameLine();
	if (ImGui::Button("Settings")) _uiSettingsLight = true;
	
	if (!scene->getDirLights().empty()){
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Directionnal Light");
		for(auto dirL : scene->getDirLights()) {
			ImGui::TextColored(ImVec4(0.1f, 0.1f, 1.0f, 1.0f), "Direction :"); ImGui::SameLine();
			ImGui::Text("(%f, %f, %f) ", dirL.direction.x, dirL.direction.y, dirL.direction.z); ImGui::SameLine();
			ImGui::TextColored(ImVec4(0.1f, 0.1f, 1.0f, 1.0f), "Color :"); ImGui::SameLine();
			ImGui::Text("(%f, %f, %f) ", dirL.color.x, dirL.color.y, dirL.color.z); ImGui::SameLine();
			ImGui::TextColored(ImVec4(0.1f, 0.1f, 1.0f, 1.0f), "Intensity :"); ImGui::SameLine();
			ImGui::Text("%f", dirL.intensity);
		}
	}
}

void ImGuiLayer::addLight(Scene* scene) {
	static float color[3] = {1.0f, 1.0f, 1.0f};
	static float direction[3] = {0.0f, -1.0f, 0.0f};
	static float intensity = 1.0f;
	
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
					break ;
				}
			}
			_uiAddLight = false;
		}
			
		ImGui::End();
}

void ImGuiLayer::settingsLight(Scene* scene) {
	ImGui::Begin("Settings");

	if (ImGui::Button("Ok")) _uiSettingsLight = false;
	ImGui::End();
}

	// ---Error---
void ImGuiLayer::renderError() {
	if (ImGui::BeginPopupModal("Error", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), _errorMessage.c_str());

		ImGui::Separator();

		if (ImGui::Button("OK"))
			ImGui::CloseCurrentPopup();

		ImGui::EndPopup();
	}
}