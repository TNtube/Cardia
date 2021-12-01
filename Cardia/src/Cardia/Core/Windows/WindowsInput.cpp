#include "cdpch.hpp"
#include "Cardia/Core/Input.hpp"

#include <GLFW/glfw3.h>


namespace Cardia
{
	inline GLFWwindow* getNativeWin()
	{
		return static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWin());
	}

	bool Input::isKeyPressed(int keyCode) {
		auto state = glfwGetKey(getNativeWin(), keyCode);
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool Input::isMouseButtonPressed(int button) {
		return glfwGetMouseButton(getNativeWin(), button) == GLFW_PRESS;
	}

	std::pair<float, float> Input::getMousePos() {
		double x, y;
		glfwGetCursorPos(getNativeWin(), &x, &y);

		return { static_cast<float>(x), static_cast<float>(y) };
	}

	float Input::getMouseX() {
		return getMousePos().first;
	}

	float Input::getMouseY() {
		return getMousePos().second;
	}
}
