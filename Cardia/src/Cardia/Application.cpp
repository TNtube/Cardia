#include "cdpch.hpp"
#include "Cardia/Application.hpp"
#include "Cardia/Renderer/Renderer2D.hpp"
#include "Cardia/Scripting/ScriptEngine.hpp"

#include <GLFW/glfw3.h>
#include <Cardia/Asset/AssetsManager.hpp>

namespace Cardia
{
	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		CdCoreAssert(!s_Instance, "Application already exists");
		s_Instance = this;
		m_Window->SetEventCallback([this](Event& e)
		{
			EventDispatcher dispatcher(e);
			dispatcher.dispatch<WindowCloseEvent>(CD_BIND_EVENT_FN(Application::OnWinClose));
			OnEvent(e);
		});
		
		Renderer2D::init();
	}

	Application::~Application()
	{
		Renderer2D::quit();
	};

	void Application::Run()
	{
		while (m_Running)
		{
			Time::m_DeltaTime = static_cast<float>(glfwGetTime()) - Time::m_LastFrameTime;
			Time::m_LastFrameTime += Time::m_DeltaTime.AsSeconds();
			m_Window->OnUpdate();

			OnUpdate();

			OnRender();

			m_AssetsManager.Update();
		}
	}

	bool Application::OnWinClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}
}
