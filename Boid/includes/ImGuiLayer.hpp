/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ImGuiLayer.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lde-merc <lde-merc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/23 10:10:56 by lde-merc          #+#    #+#             */
/*   Updated: 2026/03/23 11:43:40 by lde-merc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <ImGui/imgui_impl_glfw.h>
#include <ImGui/imgui_impl_opengl3.h>

#include "ComputeShader.hpp"
#include "BoidSimulation.hpp"
#include "Renderer.hpp"

class ImGuiLayer {
	public:
		ImGuiLayer();
		~ImGuiLayer();

		void init(GLFWwindow *);
		void beginFrame();
		void render(BoidSimulation&, Renderer&);
		void endFrame();
		void shutdown();

		void renderGlobal(BoidSimulation&, Renderer& );
		void renderBoid(BoidSimulation&);

	private:
		
};
