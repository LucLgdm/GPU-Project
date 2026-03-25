/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ImGuiLayer.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/23 10:11:11 by lde-merc          #+#    #+#             */
/*   Updated: 2026/03/25 13:16:44 by lde-merc         ###   ########.fr       */
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

	ImGui::StyleColorsClassic();
	
	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowRounding = 8.0f;
	style.FrameRounding  = 6.0f;
	style.GrabRounding   = 10.0f;
	style.ScrollbarSize = 5.0f;
	style.ScrollbarRounding = 6.0f;
	
	ImVec4* colors = style.Colors;
	colors[ImGuiCol_SliderGrab]        = ImVec4(0.80f,0.50f,0.20f,1.00f);
	colors[ImGuiCol_SliderGrabActive]  = ImVec4(1.00f,0.60f,0.20f,1.00f);
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
	
	renderGlobal(bs, render);
	renderBoid(bs);
	renderObject(bs, render);

	endFrame(); 
}

void ImGuiLayer::renderGlobal(BoidSimulation& bs, Renderer& render) {
	ImGui::Begin("General");

	static float uiBoxSize = render.getBoxSize();
	bool boxSizeChanged  = ImGui::SliderFloat("BoxSize", &uiBoxSize, 3.0f, 20.0f);
	if (boxSizeChanged) {
		render.updateBoxSize(uiBoxSize);
		render.initBox();
		bs.updateBoundSize(uiBoxSize);
	}
	
	ImGui::End();
}

void ImGuiLayer::renderBoid(BoidSimulation& bs) {
	ImGui::Begin("Boids");	
	
	// Speed
	ComputeShader& cs = bs.getCS();
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Speed");
	static float uiMaxSpeed = cs.getMaxSpeed();
	static float uiMinSpeed = cs.getMinSpeed();
	static float uiTurnSpeed = cs.getTurnSpeed();
	// bool maxSpeedchanged  = ImGui::DragFloat("Max speed", &uiMaxSpeed, 0.5f, 1.0f, 15.0f);
	bool maxSpeedchanged  = ImGui::SliderFloat("Max speed", &uiMaxSpeed, 0.1f, 100.0f, "%.2f", ImGuiSliderFlags_Logarithmic);
	bool minSpeedchanged  = ImGui::SliderFloat("Min speed", &uiMinSpeed, 0.01f, uiMaxSpeed);
	bool turnSpeedchanged = ImGui::SliderFloat("Turn speed", &uiTurnSpeed, 1.0f, 50.0f);
	
	if (maxSpeedchanged) cs.setMaxSpeed(uiMaxSpeed);
	if (minSpeedchanged) cs.setMinSpeed(uiMinSpeed);
	if (turnSpeedchanged) cs.setTurnSpeed(uiTurnSpeed);
	
	// Rules
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Rules");
	static float uiSepRadius = cs.getSepRad();
	static float uiSepWeight = cs.getSepWei();

	bool sepRadiusChanged = ImGui::SliderFloat("Separation radius", &uiSepRadius, 0.1f, 5.0f);
	bool sepWeightChanged = ImGui::SliderFloat("Separation weight", &uiSepWeight, 0.5f, 2.0f);
	if (sepRadiusChanged) cs.setSepRad(uiSepRadius);
	if (sepWeightChanged) cs.setSepWei(uiSepWeight);

	static float uiAlignRadius = cs.getAlignRad();
	static float uiAlignWeight = cs.getAlignWei();
	
	bool alignRadiusChanged = ImGui::SliderFloat("Alignment radius", &uiAlignRadius, 0.1f, 5.0f);
	bool alignWeightChanged = ImGui::SliderFloat("Alignment weight", &uiAlignWeight, 0.5f, 2.0f);
	if (alignRadiusChanged) cs.setAlignRad(uiAlignRadius);
	if (alignWeightChanged) cs.setAlignWei(uiAlignWeight);

	static float uiCoheRadius = cs.getCoheRad();
	static float uiCoheWeight = cs.getCoheWei();

	bool coheRadiusChanged = ImGui::SliderFloat("Cohesion radius", &uiCoheRadius, 0.1f, 5.0f);
	bool coheWeightChanged = ImGui::SliderFloat("Cohesion weight", &uiCoheWeight, 0.5f, 2.0f);
	if (coheRadiusChanged) cs.setCoheRad(uiCoheRadius);
	if (coheWeightChanged) cs.setCoheWei(uiCoheWeight);

	ImGui::End();
}

static void sphereOption(BoidSimulation& bs) {
	std::vector<Sphere> sphere = bs.getSphere();
	int numSphere = sphere.size();
	static bool initializedSp = false;
	static glm::vec4 editValues[128];

	for (size_t i = 0; i < sphere.size(); ++i) {
		// auto& sp = sphere[i];
		if (!initializedSp) {
			for (size_t j = 0; j < sphere.size(); ++j)
				editValues[j] = sphere[j].positionRadius;
			initializedSp = true;
		}

		ImGui::Separator();
		ImGui::Text("Sphere %zu", i); ImGui::SameLine();

		ImGui::PushItemWidth(70);

		ImGui::InputFloat(("X##" + std::to_string(i)).c_str(), &editValues[i].x); ImGui::SameLine();
		ImGui::InputFloat(("Y##" + std::to_string(i)).c_str(), &editValues[i].y); ImGui::SameLine();
		ImGui::InputFloat(("Z##" + std::to_string(i)).c_str(), &editValues[i].z); ImGui::SameLine();
		ImGui::InputFloat(("R##" + std::to_string(i)).c_str(), &editValues[i].w); ImGui::SameLine();

		ImGui::PopItemWidth();

		if (ImGui::Button(("Update##Sphere" + std::to_string(i)).c_str())) {
			// sp.positionRadius = editValues[i];
			bs.updateSpherePos(editValues[i], i);
		}
	}


	ImGui::Text("Add a sphere");
	static glm::vec4 newSphere(0.0f);
	
	ImGui::PushItemWidth(70);

	ImGui::InputFloat("X##", &newSphere.x); ImGui::SameLine();
	ImGui::InputFloat("Y##", &newSphere.y); ImGui::SameLine();
	ImGui::InputFloat("Z##", &newSphere.z); ImGui::SameLine();
	ImGui::InputFloat("R##", &newSphere.w); ImGui::SameLine();

	ImGui::PopItemWidth();
	
	if (ImGui::Button("Add##sphere")) {
		bs.addSphere(glm::vec3(newSphere), newSphere.w);
		newSphere = glm::vec4(0.0f); // reset les champs
		initializedSp = false; // force la réinitialisation de editValues
	}
}

static void cubeOption(BoidSimulation& bs) {
	std::vector<Cube> cube = bs.getCube();
	int numCube = cube.size();

	static bool initializedCu = false;
	static glm::vec4 editValuesMin[128];
	static glm::vec4 editValuesMax[128];

	for (size_t i = 0; i < cube.size(); ++i) {
		if (!initializedCu) {
			for (size_t j = 0; j < cube.size(); ++j) {
				editValuesMin[j] = cube[j].min;
				editValuesMax[j] = cube[j].max;
			}
			initializedCu = true;
		}

		ImGui::Separator();
		ImGui::Text("Cube %zu", i); ImGui::SameLine();

		ImGui::PushItemWidth(70);

		ImGui::InputFloat(("Min X##" + std::to_string(i)).c_str(), &editValuesMin[i].x); ImGui::SameLine();
		ImGui::InputFloat(("Min Y##" + std::to_string(i)).c_str(), &editValuesMin[i].y); ImGui::SameLine();
		ImGui::InputFloat(("Min Z##" + std::to_string(i)).c_str(), &editValuesMin[i].z); 
		ImGui::InputFloat(("Max X##" + std::to_string(i)).c_str(), &editValuesMax[i].x); ImGui::SameLine();
		ImGui::InputFloat(("Max Y##" + std::to_string(i)).c_str(), &editValuesMax[i].y); ImGui::SameLine();
		ImGui::InputFloat(("Max Z##" + std::to_string(i)).c_str(), &editValuesMax[i].z); ImGui::SameLine();

		ImGui::PopItemWidth();

		if (ImGui::Button(("Update##Cube" + std::to_string(i)).c_str())) {
			bs.updateCubePos(editValuesMin[i], editValuesMax[i], i);
		}
	}


	ImGui::Text("Add a cube");
	static glm::vec4 newCubeMin(0.0f);
	static glm::vec4 newCubeMax(0.0f);
	
	ImGui::PushItemWidth(70);
	ImGui::Text("	Min");
	ImGui::InputFloat("X##CubeMinX", &newCubeMin.x); ImGui::SameLine();
	ImGui::InputFloat("Y##CubeMinY", &newCubeMin.y); ImGui::SameLine();
	ImGui::InputFloat("Z##CubeMinZ", &newCubeMin.z);
	
	ImGui::Text("	Max");
	ImGui::InputFloat("X##CubeMaxX", &newCubeMax.x); ImGui::SameLine();
	ImGui::InputFloat("Y##CubeMaxY", &newCubeMax.y); ImGui::SameLine();
	ImGui::InputFloat("Z##CubeMaxZ", &newCubeMax.z); ImGui::SameLine();

	ImGui::PopItemWidth();
	
	if (ImGui::Button("Add##Cube")) {
		bs.addCube(newCubeMin, newCubeMax);
		newCubeMin = glm::vec4(0.0f); // reset les champs
		newCubeMax = glm::vec4(0.0f); // reset les champs
		initializedCu = false; // force la réinitialisation de editValues
	}
}

static void toreOption(BoidSimulation& bs) {

	std::vector<Tore> torus = bs.getTore();
	int numTorus = torus.size();

	static bool initializedTo = false;
	static glm::vec4 editPosition[128];
	static glm::vec4 editRadii[128];

	for (size_t i = 0; i < torus.size(); ++i) {

		if (!initializedTo) {
			for (size_t j = 0; j < torus.size(); ++j) {
				editPosition[j] = torus[j].position;
				editRadii[j]    = torus[j].radii;
			}
			initializedTo = true;
		}

		ImGui::Separator();
		ImGui::Text("Torus %zu", i);

		ImGui::PushItemWidth(70);

		// Position
		ImGui::Text("Position");
		ImGui::InputFloat(("X##TPosX" + std::to_string(i)).c_str(), &editPosition[i].x); ImGui::SameLine();
		ImGui::InputFloat(("Y##TPosY" + std::to_string(i)).c_str(), &editPosition[i].y); ImGui::SameLine();
		ImGui::InputFloat(("Z##TPosZ" + std::to_string(i)).c_str(), &editPosition[i].z);

		// Radii
		ImGui::Text("Radii");
		ImGui::InputFloat(("R major##" + std::to_string(i)).c_str(), &editRadii[i].x); ImGui::SameLine();
		ImGui::InputFloat(("R minor##" + std::to_string(i)).c_str(), &editRadii[i].y); ImGui::SameLine();

		ImGui::PopItemWidth();

		if (ImGui::Button(("Update##Torus" + std::to_string(i)).c_str())) {
			bs.updateTorePos(editPosition[i], editRadii[i], i);
		}
	}

	// ==========================
	// Add new torus
	// ==========================

	ImGui::Separator();
	ImGui::Text("Add a Torus");

	static glm::vec4 newPos(0.0f);
	static glm::vec4 newRadii(0.5f, 0.2f, 0.0f, 0.0f);

	ImGui::PushItemWidth(70);

	ImGui::Text("Position");
	ImGui::InputFloat("X##NewTPosX", &newPos.x); ImGui::SameLine();
	ImGui::InputFloat("Y##NewTPosY", &newPos.y); ImGui::SameLine();
	ImGui::InputFloat("Z##NewTPosZ", &newPos.z);

	ImGui::Text("Radii");
	ImGui::InputFloat("R major##NewTRMajor", &newRadii.x); ImGui::SameLine();
	ImGui::InputFloat("R minor##NewTRMinor", &newRadii.y); ImGui::SameLine();

	ImGui::PopItemWidth();

	if (ImGui::Button("Add##Torus")) {
		bs.addTore(newPos, newRadii);
		newPos   = glm::vec4(0.0f);
		newRadii = glm::vec4(0.5f, 0.2f, 0.0f, 0.0f);
		initializedTo = false;
	}
}

void ImGuiLayer::renderObject(BoidSimulation& bs, Renderer& render) {
	ImGui::Begin("Obstacle");
	// Sphere
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Sphere");
	sphereOption(bs);
	
	// Cube
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Cube");
	cubeOption(bs);
	
	// Tore
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Tore");
	toreOption(bs);
	
	ImGui::End();
}