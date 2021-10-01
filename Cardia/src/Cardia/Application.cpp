#include "cdpch.hpp"
#include "Cardia/Application.hpp"
#include <GLFW/glfw3.h>


namespace Cardia
{
	Application* Application::s_Instance = nullptr;

	Application::Application() : m_DeltaTime()
	{
		cdCoreAssert(!s_Instance, "Application already exists");
		s_Instance = this;

		m_Window = Window::Create();
		m_Window->setEventCallback(CD_BIND_EVENT_FN(Application::onEvent));

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
	}

	void Application::onEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.dispatch<WinCloseEvent>(CD_BIND_EVENT_FN(Application::onWinClose));

		for (auto it = m_LayerStack.rbegin(); it < m_LayerStack.rend(); ++it)
		{
			(*it)->onEvent(e);
			if(e.isHandled())
				break;
		}
	}
	
	void Application::Run()
	{
		float time = 0.0f;
		while (m_Running)
		{
			m_DeltaTime = static_cast<float>(glfwGetTime()) - time;
			time += m_DeltaTime.seconds();

			for (const auto layer : m_LayerStack)
			{
				layer->onUpdate(m_DeltaTime);
			}

			m_ImGuiLayer->Begin();
			for (const auto layer : m_LayerStack)
			{
				layer->onImGuiDraw(m_DeltaTime);
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
