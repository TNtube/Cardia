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

		m_Renderer = std::make_unique<Renderer>();

		m_ImGuiLayer = std::make_unique<ImGuiLayer>();
		pushOverlay(m_ImGuiLayer.get());

		m_VertexArray.reset(VertexArray::create());

		float vertices[] = {
			-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
			 0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f
		};

		uint32_t indices[] {0, 1, 2};

		std::shared_ptr<VertexBuffer> vertexBuffer;
		vertexBuffer.reset(VertexBuffer::create(vertices, sizeof(vertices)));


		BufferLayout layout = {
			{ShaderDataType::Float3, "position"},
			{ShaderDataType::Float4, "col"}
		};

		vertexBuffer->setLayout(layout);
		m_VertexArray->addVertexBuffer(vertexBuffer);

		std::shared_ptr<IndexBuffer> indexBuffer;
		indexBuffer.reset(IndexBuffer::create(indices, sizeof(indices) / sizeof(uint32_t)));
		m_VertexArray->setIndexBuffer(indexBuffer);

		std::string vertexSrc = R"(
			#version 440 core

			layout(location = 0) in vec3 position;
			layout(location = 1) in vec4 col;

			out vec3 o_Pos;
			out vec4 o_Col;

			void main() {
				o_Pos = position;
				o_Col = col;
				gl_Position = vec4(position, 1.0f);
			}
			)";

		std::string fragmentSrc = R"(
			#version 440 core

			layout(location = 0) out vec4 color;

			in vec3 o_Pos;
			in vec4 o_Col;

			void main() {
			   color = o_Col;
			}
		)";

		m_Shader.reset(Shader::create(vertexSrc, fragmentSrc));
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

			m_Shader->bind();
			m_VertexArray->bind();
			glDrawElements(GL_TRIANGLES, m_VertexArray->getIndexBuffer()->getCount(), GL_UNSIGNED_INT, nullptr);

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
