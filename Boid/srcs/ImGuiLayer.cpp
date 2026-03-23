/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ImGuiLayer.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/23 10:11:11 by lde-merc          #+#    #+#             */
/*   Updated: 2026/03/23 12:43:25 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ImGuiLayer.hpp"

ImGuiLayer::ImGuiLayer() { }

ImGuiLayer::~ImGuiLayer() { }

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

void ImGuiLayer::endFrame() {
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

// Cleanup ImGui resources
void ImGuiLayer::shutdown() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void ImGuiLayer::render(BoidSimulation& bs, Renderer& render) {
	beginFrame();
	ImGui::Begin("Controls");	

	renderGlobal(bs, render);
	renderBoid(bs);
	
	ImGui::End();
	endFrame(); 
}

void ImGuiLayer::renderGlobal(BoidSimulation& bs, Renderer& render) {
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "General information");

	static float uiBoxSize = render.getBoxSize();
	bool boxSizeChanged  = ImGui::DragFloat("BoxSize", &uiBoxSize, 1);
	if (boxSizeChanged) {
		render.updateBoxSize(uiBoxSize);
		render.initBox();
		bs.updateBoundSize(uiBoxSize);
	}
}


void ImGuiLayer::renderBoid(BoidSimulation& bs) {
	ComputeShader& cs = bs.getCS();
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Boids informations");
	// Speed
	static float uiMaxSpeed = cs.getMaxSpeed();
	static float uiMinSpeed = cs.getMinSpeed();
	static float uiTurnSpeed = cs.getTurnSpeed();
	// bool maxSpeedchanged  = ImGui::DragFloat("Max speed", &uiMaxSpeed, 0.5f, 1.0f, 15.0f);
	bool maxSpeedchanged  = ImGui::SliderFloat("Max speed", &uiMaxSpeed, 0.1f, 100.0f, "%.2f", ImGuiSliderFlags_Logarithmic);
	bool minSpeedchanged  = ImGui::DragFloat("Min speed", &uiMinSpeed, 0.01f, 0.01f, 5.0f);
	bool turnSpeedchanged = ImGui::DragFloat("Turn speed", &uiTurnSpeed, 0.5f, 1.0f, 50.0f);
	
	if (maxSpeedchanged) cs.setMaxSpeed(uiMaxSpeed);
	if (minSpeedchanged) cs.setMinSpeed(uiMinSpeed);
	if (turnSpeedchanged) cs.setTurnSpeed(uiTurnSpeed);
	
	// Rules
	static float uiSepRadius = cs.getSepRad();
	static float uiSepWeight = cs.getSepWei();

	bool sepRadiusChanged = ImGui::DragFloat("Separation radius", &uiSepRadius, 0.1f, 0.1f, 5.0f);
	bool sepWeightChanged = ImGui::DragFloat("Separation weight", &uiSepWeight, 0.1f, 0.1f, 5.0f);
	if (sepRadiusChanged) cs.setSepRad(uiSepRadius);
	if (sepWeightChanged) cs.setSepWei(uiSepWeight);

	static float uiAlignRadius = cs.getAlignRad();
	static float uiAlignWeight = cs.getAlignWei();
	
	bool alignRadiusChanged = ImGui::DragFloat("Alignment radius", &uiAlignRadius, 0.1f, 0.1f, 5.0f);
	bool alignWeightChanged = ImGui::DragFloat("Alignment weight", &uiAlignWeight, 0.1f, 0.1f, 5.0f);
	if (alignRadiusChanged) cs.setAlignRad(uiAlignRadius);
	if (alignWeightChanged) cs.setAlignWei(uiAlignWeight);

	static float uiCoheRadius = cs.getCoheRad();
	static float uiCoheWeight = cs.getCoheWei();

	bool coheRadiusChanged = ImGui::DragFloat("Cohesion radius", &uiCoheRadius, 0.1f, 0.1f, 5.0f);
	bool coheWeightChanged = ImGui::DragFloat("Cohesion weight", &uiCoheWeight, 0.1f, 0.1f, 5.0f);
	if (coheRadiusChanged) cs.setCoheRad(uiCoheRadius);
	if (coheWeightChanged) cs.setCoheWei(uiCoheWeight);
}