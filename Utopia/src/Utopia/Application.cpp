#include "utpch.hpp"
#include "Application.hpp"

#include <glad/glad.h>


namespace Utopia
{
	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		utCoreAssert(!s_Instance, "Application already exists");
		s_Instance = this;

		m_Window = Window::Create();
		m_Window->setEventCallback(UT_BIND_EVENT_FN(Application::onEvent));

		m_ImGuiLayer = std::make_unique<ImGuiLayer>();
		pushOverlay(m_ImGuiLayer.get());
	}

	void Application::pushLayer(Layer* layer)
	{
		m_LayerStack.pushLayer(layer);
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
			glClearColor(0.2f, 0.2f, 0.2f, 1);
			glClear(GL_COLOR_BUFFER_BIT);
			for (const auto layer : m_LayerStack)
			{
				layer->onUpdate();
			}

			m_ImGuiLayer->Begin();
			for (const auto layer : m_LayerStack)
			{
				layer->onImGuiDraw();
			}
			m_ImGuiLayer->End();
			m_Window->onUpdate();

		}
	}

	bool Application::onWinClose(WinCloseEvent& e)
	{
		m_Running = false;
		return true;
	}
}
