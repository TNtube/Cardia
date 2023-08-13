#pragma once
#include <imgui.h>

#include "Cardia/Math/Vector2.hpp"
#include "Cardia/Math/Vector3.hpp"
#include "Cardia/Math/Vector4.hpp"

namespace Cardia::EditorUI
{
	bool DragInt(const char* label, int* data, float speed = 1.0f, int v_min = INT_MIN, int v_max = INT_MAX);
	bool DragFloat(const char* label, float* data, float speed = 1.0f, float v_min = FLT_MIN, float v_max = FLT_MAX);
	bool InputText(const char* label, std::string* str, const Vector4f& color = Vector4f(1), ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = nullptr, void* user_data  = nullptr);

	bool ColorEdit3(const char* label, float col[3], ImGuiColorEditFlags flags = 0);
	bool ColorEdit4(const char* label, float col[4], ImGuiColorEditFlags flags = 0);

	bool DragFloat2(const std::string& label, Vector2f& vector, float reset = 0.0f);
	bool DragFloat3(const std::string& label, Vector3f& vector, float reset = 0.0f);
	bool DragFloat4(const std::string& label, Vector4f& vector, float reset = 0.0f);

	bool Combo(const char* label, int* current_item, const char* const items[], int items_count, int popup_max_height_in_items = -1);

	bool Checkbox(const char* label, bool* v);
}
