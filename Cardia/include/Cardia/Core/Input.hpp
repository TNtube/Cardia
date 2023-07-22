#pragma once
#include <glm/glm.hpp>

#include "Cardia/Application.hpp"


namespace Cardia
{
	class Input
	{
	public:
		static bool IsKeyPressed(int keyCode);
		static bool IsMouseButtonPressed(int button);
		static glm::vec2 GetMousePos();
		static float GetMouseX();
		static float GetMouseY();
	};
}