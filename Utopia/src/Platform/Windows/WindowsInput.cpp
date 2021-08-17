#include "utpch.hpp"
#include "Utopia/Input.hpp"

#include <GLFW/glfw3.h>

namespace Utopia
{
	bool Input::isKeyPressed(int keyCode) {
		auto window = static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWin());
		auto state = glfwGetKey(window, keyCode);
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool Input::isMouseButtonPressed(int button) {
		auto window = static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWin());
		auto state = glfwGetMouseButton(window, button);
		return state == GLFW_PRESS;
	}

	std::pair<float, float> Input::getMousePos() {
		auto window = static_cast<GLFWwindow*>(Application::get().getWindow().getNativeWin());
		double x, y;
		glfwGetCursorPos(window, &x, &y);

		return { static_cast<float>(x), static_cast<float>(y) };
	}

	float Input::getMouseX() {
		auto[x, y] = getMousePos();
		return x;
	}

	float Input::getMouseY() {
		auto[x, y] = getMousePos();
		return y;
	}
}