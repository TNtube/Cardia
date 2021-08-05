#include "utpch.hpp"
#include "Application.hpp"

#include "Utopia/Log.hpp"

#include <GLFW/glfw3.h>
#include <gl/GL.h>


namespace Utopia
{
	Application::Application()
	{
		m_Window = std::unique_ptr<Window>(Window::Create());
	}
	
	Application::~Application()
	{

	}

	void Application::Run()
	{
		while (m_Running)
		{
			glClearColor(1, 1, 0, 1);
			glClear(GL_COLOR_BUFFER_BIT);
			m_Window->onUpdate();
		}
	}
}
