/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ImGuiLayer.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 12:56:58 by lde-merc          #+#    #+#             */
/*   Updated: 2026/06/10 17:38:39 by lde-merc         ###   ########.fr       */
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

	// --Render--
void ImGuiLayer::render(Scene* scene) {
	ImGui::Begin("Hello ImGui");
	
	sceneLoader(scene);
	displayListObject(scene);
	ImGui::End();

	if (_showErrorPopup) {
		ImGui::OpenPopup("Error");
		_showErrorPopup = false;
	}
	renderError();
}

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
	if (ImGui::BeginTable("SceneObjects", 1)) {
		for (int i = 0; i < scene->getObjects().size(); ++i) {
			auto& object = scene->getObjects()[i];

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);

			ImGui::TableSetColumnIndex(1);

			if (ImGui::Button(("Delete##" + std::to_string(i)).c_str()))
			{
				scene.removeObject(i);
				break;
			}
		
			bool selected = (selectedObject == i);

			if (ImGui::Selectable(scene->getObjName()[i].c_str(), selected))
				selectedObject = i;
		}
		ImGui::EndTable();
	}

	if (selectedObject >= 0) {
		ImGui::Begin("Properties");
		
		auto& object = scene->getObjects()[selectedObject];

		ImGui::Separator();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Object Properties");

		ImGui::Text("Name: %s", scene->getObjName()[selectedObject].c_str());
		ImGui::Text("Triangles: %llu", static_cast<long long int>(count));

		if (ImGui::Button("OK"))
			selectedObject = -1;
		ImGui::End();
	}
}


void ImGuiLayer::renderError() {
	if (ImGui::BeginPopupModal("Error", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), _errorMessage.c_str());

		ImGui::Separator();

		if (ImGui::Button("OK"))
			ImGui::CloseCurrentPopup();

		ImGui::EndPopup();
	}
}