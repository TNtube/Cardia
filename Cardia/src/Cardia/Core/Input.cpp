#include "cdpch.hpp"
#include "Cardia/Core/Input.hpp"

#include <GLFW/glfw3.h>


namespace Cardia
{
	static GLFWwindow* GetNativeWin()
	{
		return static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
	}

	bool Input::IsKeyPressed(int keyCode) {
		auto state = glfwGetKey(GetNativeWin(), keyCode);
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool Input::IsMouseButtonPressed(int button) {
		return glfwGetMouseButton(GetNativeWin(), button) == GLFW_PRESS;
	}

	glm::vec2 Input::GetMousePos() {
		double x, y;
		glfwGetCursorPos(GetNativeWin(), &x, &y);

		return { static_cast<float>(x), static_cast<float>(y) };
	}

	float Input::GetMouseX() {
		return GetMousePos().x;
	}

	float Input::GetMouseY() {
		return GetMousePos().y;
	}
}
