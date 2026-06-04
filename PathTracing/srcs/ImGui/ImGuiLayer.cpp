/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ImGuiLayer.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 12:56:58 by lde-merc          #+#    #+#             */
/*   Updated: 2026/06/04 13:03:31 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ImGuiLayer.hpp"

ImGuiLayer::ImGuiLayer() {}

ImGuiLayer::~ImGuiLayer() {}

void ImGuiLayer::init(GLFWwindow* window) {
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui::StyleColorsDark();
	ImGui_ImplOpenGL3_Init("#version 430");
}

void ImGuiLayer::beginFrame() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void ImGuiLayer::render() {
	ImGui::Begin("Hello ImGui");
	
	ImGui::End();
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
