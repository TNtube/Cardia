#pragma once
#include <glm/glm.hpp>

#include "Cardia/Application.hpp"


namespace Cardia
{
	class Input
	{
	public:
		static bool isKeyPressed(int keyCode);
		static bool isMouseButtonPressed(int button);
		static glm::vec2 getMousePos();
		static float getMouseX();
		static float getMouseY();
	};
}