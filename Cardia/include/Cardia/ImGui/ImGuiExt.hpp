#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>

namespace Cardia::ImGuiExt
{
	inline void InputVec3(const char* label, glm::vec3& vec3, float speed = 1.0f)
	{
		ImGui::DragFloat3(label, glm::value_ptr(vec3), speed);
	}
}
