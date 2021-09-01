#pragma once

#include "Cardia/Core.hpp"
#include "Cardia/Application.hpp"


namespace Cardia
{
	class Input
	{
	public:
		static bool isKeyPressed(int keyCode);
		static bool isMouseButtonPressed(int keyCode);
		static std::pair<float, float> getMousePos();
		static float getMouseX();
		inline static float getMouseY();
	};
}