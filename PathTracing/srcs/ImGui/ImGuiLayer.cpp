/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ImGuiLayer.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 12:56:58 by lde-merc          #+#    #+#             */
/*   Updated: 2026/06/09 16:52:59 by lde-merc         ###   ########.fr       */
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
	
	ImGui::End();
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
				_error = 1;
			}
		}

		ImGuiFileDialog::Instance()->Close();
	}
}

void ImGuiLayer::renderError() {
	ImGui::Begin("Error");
	switch (_error){
		case 1:
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Error\nObject is already loaded");
			break ;
		default:
			break ;
	}

	if (ImGui::Button("OK")) _error = 0;
	ImGui::End();
}