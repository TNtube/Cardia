#pragma once


#include "Cardia/Application.hpp"


namespace Cardia
{
	class Input
	{
	public:
		static bool isKeyPressed(int keyCode);
		static bool isMouseButtonPressed(int button);
		static std::pair<float, float> getMousePos();
		static float getMouseX();
		static float getMouseY();
	};
}