#pragma once
#include <imgui.h>
#include <glm/glm.hpp>

namespace Cardia::EditorUI
{
	bool DragInt(const char* label, int* data, float speed = 1.0f, int v_min = INT_MIN, int v_max = INT_MAX);
	bool DragFloat(const char* label, float* data, float speed = 1.0f, float v_min = FLT_MIN, float v_max = FLT_MAX);
	bool InputText(const char* label, char* buffer, std::size_t size, ImGuiInputTextFlags flags = 0);

	bool ColorEdit3(const char* label, float col[3], ImGuiColorEditFlags flags = 0);
	bool ColorEdit4(const char* label, float col[4], ImGuiColorEditFlags flags = 0);

	void DragFloat3(const std::string& label, glm::vec3& vector, float reset = 0.0f);

	bool Combo(const char* label, int* current_item, const char* const items[], int items_count, int popup_max_height_in_items = -1);

	bool Checkbox(const char* label, bool* v);
}