#include "utpch.hpp"
#include "WindowsWin.hpp"

#include <GLFW/glfw3.h>

namespace Utopia
{
	static bool isGlfwInit = false;					// Can only initialize glfw once.

	Window* Window::Create(const WinProperties& properties)
	{
		return new WindowsWin(properties);
	}

	WindowsWin::WindowsWin(const WinProperties& properties)
	{
		init(properties);
	}

	WindowsWin::~WindowsWin()
	{
		quit();
	}


	void WindowsWin::init(const WinProperties& properties)
	{
		m_Data.title = properties.title;
		m_Data.width = properties.width;
		m_Data.height = properties.height;

		if (!isGlfwInit)
		{
			int result = glfwInit();
			utAssert(result, "Can't initialize glfw3.");

			isGlfwInit = true;
		}

		m_Window = glfwCreateWindow(static_cast<int>(properties.width), 
			static_cast<int>(properties.height), 
			properties.title.c_str(), 
			nullptr, nullptr);

		glfwMakeContextCurrent(m_Window);
		glfwSetWindowUserPointer(m_Window, &m_Data);
		setVSync(true);
	}


	void WindowsWin::quit()
	{
		glfwDestroyWindow(m_Window);
	}

	void WindowsWin::onUpdate()
	{
		glfwPollEvents();
		glfwSwapBuffers(m_Window);
	}

	void WindowsWin::setVSync(bool state)
	{
		glfwSwapInterval(static_cast<int>(state));		// casting the bool give us the actual value wanted
		m_Data.vSync = state;
	}

	
	bool WindowsWin::isVSync() const
	{
		return m_Data.vSync;
	}
}
