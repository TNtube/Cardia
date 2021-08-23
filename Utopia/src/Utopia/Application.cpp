#include "utpch.hpp"
#include "Application.hpp"

#include <glad/glad.h>


namespace Utopia
{
	Application* Application::s_Instance = nullptr;

	static GLenum ShaderDataTypeToOpenGLType(ShaderDataType type)
	{
		switch (type)
		{
			case ShaderDataType::Float:
			case ShaderDataType::Float2:
			case ShaderDataType::Float3:
			case ShaderDataType::Float4:
			case ShaderDataType::Mat3:
			case ShaderDataType::Mat4:
				return GL_FLOAT;
			case ShaderDataType::Int:
			case ShaderDataType::Int2:
			case ShaderDataType::Int3:
			case ShaderDataType::Int4:
				return GL_INT;
			case ShaderDataType::Bool:
				return GL_BOOL;
			default:
				utCoreAssert(false, "Unknown ShaderDataType.");
				return 0;
		}
	}

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

		float vertices[] = {
			-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
			 0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f
		};

		unsigned indices[] {0, 1, 2};

		m_VertexBuffer.reset(VertexBuffer::create(vertices, sizeof(vertices)));


		BufferLayout layout = {
			{ShaderDataType::Float3, "position"},
			{ShaderDataType::Float4, "col"}
		};

		m_VertexBuffer->setLayout(layout);

		unsigned index = 0;
		for (const auto& element : m_VertexBuffer->getLayout())
		{
			glEnableVertexAttribArray(index);
			glVertexAttribPointer(index,
					      element.getElementCount(),
					      ShaderDataTypeToOpenGLType(element.type),
					      element.normalized ? GL_TRUE : GL_FALSE,
					      layout.getStride(),
					      reinterpret_cast<const void*>(element.offset));
			index++;
		}

		m_IndexBuffer.reset(IndexBuffer::create(indices, sizeof(indices) / sizeof(unsigned)));

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
			glBindVertexArray(m_VertexArray);
			glDrawElements(GL_TRIANGLES, m_IndexBuffer->getCount(), GL_UNSIGNED_INT, nullptr);

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
