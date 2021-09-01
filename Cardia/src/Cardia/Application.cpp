#include "cdpch.hpp"
#include "Application.hpp"


namespace Cardia
{
	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		cdCoreAssert(!s_Instance, "Application already exists");
		s_Instance = this;

		m_Window = Window::Create();
		m_Window->setEventCallback(CD_BIND_EVENT_FN(Application::onEvent));

		m_ImGuiLayer = std::make_unique<ImGuiLayer>();
		pushOverlay(m_ImGuiLayer.get());

		m_VertexArray.reset(VertexArray::create());

		float vertices[] = {
			-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
			 0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f
		};

		uint32_t indices[] {0, 1, 2};

		std::unique_ptr<VertexBuffer> vertexBuffer;
		vertexBuffer.reset(VertexBuffer::create(vertices, sizeof(vertices)));


		BufferLayout layout = {
			{ShaderDataType::Float3, "position"},
			{ShaderDataType::Float4, "col"}
		};

		vertexBuffer->setLayout(layout);
		m_VertexArray->addVertexBuffer(std::move(vertexBuffer));

		std::unique_ptr<IndexBuffer> indexBuffer;
		indexBuffer.reset(IndexBuffer::create(indices, sizeof(indices) / sizeof(uint32_t)));
		m_VertexArray->setIndexBuffer(std::move(indexBuffer));

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
		m_Shader->unbind();
		m_VertexArray->unbind();
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
		while (m_Running)
		{

			RenderCommand::setClearColor({0.2f, 0.2f, 0.2f, 1});
			RenderCommand::clear();

			Renderer::beginScene();

			m_Shader->bind();
			Renderer::submit(m_VertexArray);

			Renderer::endScene();

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
