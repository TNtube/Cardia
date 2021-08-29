#pragma once

#include "Utopia/Core.hpp"
#include "Utopia/Application.hpp"


namespace Utopia
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