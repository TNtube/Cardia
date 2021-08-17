#include "utpch.hpp"
#include "Application.hpp"

#include "Utopia/Log.hpp"
#include "Input.hpp"

#include <glad/glad.h>
#include <imgui.h>


namespace Utopia
{
	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		utCoreAssert(!s_Instance, "Application already exists");
		s_Instance = this;

		m_Window = Window::Create();
		m_Window->setEventCallback(UT_BIND_EVENT_FN(Application::onEvent));
	}

	Application::~Application() = default;

	void Application::pushLayer(Layer* layer)
	{
		m_LayerStack.pushLayer(layer);
		layer->onPush();
	}

	void Application::pushOverlay(Layer* overlay)
	{
		m_LayerStack.pushOverlay(overlay);
		overlay->onPush();
	}

	void Application::onEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.dispatch<WinCloseEvent>(UT_BIND_EVENT_FN(Application::onWinClose));

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
			for (const auto layer : m_LayerStack)
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
