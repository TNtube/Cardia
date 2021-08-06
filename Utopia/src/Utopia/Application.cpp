#include "utpch.hpp"
#include "Application.hpp"

#include "Utopia/Log.hpp"


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

	void Application::pushLayer(LayerPtr layer)
	{
		m_LayerStack.pushLayer(layer);
	}

	void Application::pushOverlay(LayerPtr overlay)
	{
		m_LayerStack.pushOverlay(overlay);
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
			for (const LayerPtr& layer : m_LayerStack)
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
