#pragma once

#include "Cardia/Math/Vector2.hpp"


namespace Cardia
{
	class Input
	{
	public:
		static bool IsKeyPressed(int keyCode);
		static bool IsMouseButtonPressed(int button);
		static Vector2f GetMousePos();
		static float GetMouseX();
		static float GetMouseY();
	};
}
