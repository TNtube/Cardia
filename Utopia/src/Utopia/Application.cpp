#include "utpch.hpp"
#include "Application.hpp"

#include "Utopia/Log.hpp"

#include <gl/GL.h>


namespace Utopia
{

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)
	Application::Application()
	{
		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->setEventCallback(BIND_EVENT_FN(onEvent));
	}
	
	Application::~Application()
	{

	}


	void Application::onEvent(Event& e)
	{
		EventDispatcher dispatcher(e);

		dispatcher.dispatch<WinCloseEvent>(BIND_EVENT_FN(onWinClose));
		UT_CORE_TRACE(e);
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

	bool Application::onWinClose(WinCloseEvent& e)
	{
		m_Running = false;
		return true;
	}

}
