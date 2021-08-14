#include "utpch.hpp"
#include "Application.hpp"

#include "Utopia/Log.hpp"

#include <glad/glad.h>
#include <imgui.h>


namespace Utopia
{
#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		utCoreAssert(!s_Instance, "Application already exist");
		s_Instance = this;

		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->setEventCallback(BIND_EVENT_FN(onEvent));
	}
	
	Application::~Application()
	{

	}

	void Application::pushLayer(std::shared_ptr<Layer>& layer)
	{
		m_LayerStack.pushLayer(layer);
		layer->onPush();
	}

	void Application::pushOverlay(std::shared_ptr<Layer>& overlay)
	{
		m_LayerStack.pushOverlay(overlay);
		overlay->onPush();
	}




	void Application::onEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.dispatch<WinCloseEvent>(BIND_EVENT_FN(onWinClose));

		for (auto it = m_LayerStack.rbegin(); it < m_LayerStack.rend(); ++it)
		{
			(*it)->onEvent(e);
			if(e.isHandled())
				break;
		}
	}
	
	void Application::Run()
	{
		while (m_Running)
		{
			glClearColor(1, 1, 0, 1);
			glClear(GL_COLOR_BUFFER_BIT);
			for (std::shared_ptr<Layer> layer : m_LayerStack)
			{
				layer->onUpdate();
			}
			m_Window->onUpdate();
		}
	}

	bool Application::onWinClose(WinCloseEvent& e)
	{
		m_Running = false;
		return true;
	}


}
