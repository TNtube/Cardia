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

		glGenVertexArrays(1, &m_VertexArray);
		glBindVertexArray(m_VertexArray);

		glGenBuffers(1, &m_VertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);

		float vertices[3 * 3] = {
			-0.5f, -0.5f, 0.0f,
			 0.5f, -0.5f, 0.0f,
			 0.0f,  0.5f, 0.0f
		};

		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

		glGenBuffers(1, &m_IndexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);

		unsigned indices[3] {0, 1, 2};

		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
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

			glBindVertexArray(m_VertexArray);
			glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr);

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
