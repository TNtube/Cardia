#pragma once
#include <imgui.h>

namespace Cardia::Theme
{
	inline void SetCardiaTheme()
	{
		auto& colors = ImGui::GetStyle().Colors;
		
		static ImVec4 windowBg = { 0.071f, 0.071f, 0.071f, 1.0f };
		static ImVec4 normal = { 0.427f, 0.02f, 0.02f, 1.0f };
		static ImVec4 hovered = { 0.478f, 0.196f, 0.196f, 1.0f };
		static ImVec4 active = { 0.1161f, 0.0f, 0.0f, 1.0f };
		static ImVec4 tabActive = { 0.418f, 0.15f, 0.15f, 1.0f };
		
		colors[ImGuiCol_WindowBg] = windowBg;

		colors[ImGuiCol_Header] = normal;
		colors[ImGuiCol_HeaderHovered] = hovered;
		colors[ImGuiCol_HeaderActive] = active;
		
		colors[ImGuiCol_Button] = normal;
		colors[ImGuiCol_ButtonHovered] = hovered;
		colors[ImGuiCol_ButtonActive] = active;

		colors[ImGuiCol_FrameBg] = normal;
		colors[ImGuiCol_FrameBgHovered] = hovered;
		colors[ImGuiCol_FrameBgActive] = active;

		colors[ImGuiCol_Tab] = normal;
		colors[ImGuiCol_TabHovered] = hovered;
		colors[ImGuiCol_TabActive] = normal;
		colors[ImGuiCol_TabUnfocusedActive] = tabActive;
		colors[ImGuiCol_MenuBarBg] = normal;

		colors[ImGuiCol_TitleBg] = active;
		colors[ImGuiCol_TitleBgActive] = active;
		colors[ImGuiCol_TitleBgCollapsed] = active;
	}
}
