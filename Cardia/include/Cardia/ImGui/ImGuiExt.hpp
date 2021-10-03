#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>

namespace Cardia::ImGuiExt
{
	inline void InputVec3(const char* label, glm::vec3& vec3)
	{
		ImGui::InputFloat3(label, glm::value_ptr(vec3));
	}
}
