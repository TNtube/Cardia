#include "cdpch.hpp"
#include "Cardia/Application.hpp"
#include "Cardia/Renderer/Renderer2D.hpp"

#include <GLFW/glfw3.h>


namespace Cardia
{
	Application* Application::s_Instance = nullptr;

	Application::Application() : m_DeltaTime()
	{
		cdCoreAssert(!s_Instance, "Application already exists");
		s_Instance = this;

		m_Window = Window::Create();
		m_Window->setEventCallback([this](Event& e)
		{
			EventDispatcher dispatcher(e);
			dispatcher.dispatch<WinCloseEvent>(CD_BIND_EVENT_FN(Application::onWinClose));
			m_ImGuiLayer->onEvent(e);
			onEvent(e);
		});

		m_ImGuiLayer = std::make_unique<ImGuiLayer>();
	}

	void Application::Run()
	{
		Renderer2D::Init();

		float time = 0.0f;
		while (m_Running)
		{
			m_DeltaTime = static_cast<float>(glfwGetTime()) - time;
			time += m_DeltaTime.seconds();

			onUpdate(m_DeltaTime);

			m_ImGuiLayer->Begin();
			onImGuiDraw(m_DeltaTime);
			m_ImGuiLayer->End();

			m_Window->onUpdate();

		}
		Renderer2D::Quit();
	}

	bool Application::onWinClose(WinCloseEvent& e)
	{
		m_Running = false;
		return true;
	}
}
