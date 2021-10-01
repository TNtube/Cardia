#include "Cardia.hpp"
#include <imgui.h>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Platform/OpenGL/OpenGLShader.hpp"


class LayerTest : public Cardia::Layer
{
public:
	explicit LayerTest(std::string&& name)
		: Layer(name), m_Camera(-1.6f, 1.6f, -0.9f, 0.9f), m_CameraPosition(0.0f)
	{
		m_VertexArray.reset(Cardia::VertexArray::create());

		float vertices[] = {
			-0.5f, -0.5f, 0.0f, // 1.0f, 0.0f, 0.0f, 1.0f,
			 0.5f, -0.5f, 0.0f, // 0.0f, 1.0f, 0.0f, 1.0f,
			 0.5f,  0.5f, 0.0f, // 0.0f, 0.0f, 1.0f, 1.0f,
			-0.5f,  0.5f, 0.0f  // 1.0f, 0.0f, 1.0f, 1.0f
		};

		uint32_t indices[] {
			0, 1, 2,
			2, 3, 0
		};

		std::unique_ptr<Cardia::VertexBuffer> vertexBuffer;
		vertexBuffer.reset(Cardia::VertexBuffer::create(vertices, sizeof(vertices)));


		Cardia::BufferLayout layout = {
			{Cardia::ShaderDataType::Float3, "position"}
		};

		vertexBuffer->setLayout(layout);
		m_VertexArray->addVertexBuffer(std::move(vertexBuffer));

		std::unique_ptr<Cardia::IndexBuffer> indexBuffer;
		indexBuffer.reset(Cardia::IndexBuffer::create(indices, sizeof(indices) / sizeof(uint32_t)));
		m_VertexArray->setIndexBuffer(std::move(indexBuffer));

		std::string vertexSrc = R"(
			#version 440 core

			layout(location = 0) in vec3 position;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Model;

			void main() {
				gl_Position = u_ViewProjection * u_Model * vec4(position, 1.0f);
			}
		)";

		std::string fragmentSrc = R"(
			#version 440 core

			layout(location = 0) out vec4 color;

			uniform vec3 u_Color;

			void main() {
			   color = vec4(u_Color, 1.0f);
			}
		)";

		m_Shader.reset(Cardia::Shader::create(vertexSrc, fragmentSrc));
		m_Shader->unbind();
		m_VertexArray->unbind();
	}

	void onUpdate(Cardia::DeltaTime deltaTime) override
	{
		if (Cardia::Input::isKeyPressed(Cardia::Key::Left))
			m_CameraPosition.x -= m_CameraSpeed * deltaTime.seconds();
		else if (Cardia::Input::isKeyPressed(Cardia::Key::Right))
			m_CameraPosition.x += m_CameraSpeed * deltaTime.seconds();

		if (Cardia::Input::isKeyPressed(Cardia::Key::Down))
			m_CameraPosition.y -= m_CameraSpeed * deltaTime.seconds();
		else if (Cardia::Input::isKeyPressed(Cardia::Key::Up))
			m_CameraPosition.y += m_CameraSpeed * deltaTime.seconds();

		m_Camera.setPosition(m_CameraPosition);

		Cardia::RenderCommand::setClearColor({0.2f, 0.2f, 0.2f, 1});
		Cardia::RenderCommand::clear();

		Cardia::Renderer::beginScene(m_Camera);

		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(m_Scale));

		// Extremely temporary
		dynamic_cast<Cardia::OpenGLShader&>(*m_Shader).bind();
		dynamic_cast<Cardia::OpenGLShader&>(*m_Shader).setUniformFloat3("u_Color", m_Color);

		for (int x = 0; x < 10; ++x)
		{
			for (int y = 0; y < 10; ++y)
			{
				glm::vec3 pos(static_cast<float>(x) * (m_Scale + m_Scale / 10), static_cast<float>(y) * (m_Scale + m_Scale / 10), 0.0f);
				glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;
				Cardia::Renderer::submit(m_VertexArray, m_Shader, transform);
			}
		}

		Cardia::Renderer::endScene();
	}

	void onEvent(Cardia::Event& event) override
	{
		Cardia::EventDispatcher dispatcher(event);
		dispatcher.dispatch<Cardia::MouseScrolledEvent>([this](const Cardia::MouseScrolledEvent& ev) -> bool{
			m_Scale += ev.getOffSetY() / 100;
			return false;
		});
	}

	void onImGuiDraw(Cardia::DeltaTime deltaTime) override
	{
		ImGui::Begin("Debug tools");
		ImGui::ColorEdit3("Square Color", glm::value_ptr(m_Color));
		ImGui::End();
	}

private:
	std::unique_ptr<Cardia::Shader> m_Shader;
	std::unique_ptr<Cardia::VertexArray> m_VertexArray;

	Cardia::OrthographicCamera m_Camera;

	glm::vec3 m_CameraPosition;
	float m_CameraSpeed = 2.0f;
	float m_Scale = 0.1f;
	glm::vec3 m_Color{0.2f, 0.8f, 0.3f};
};


class SandBox : public Cardia::Application
{
public:
	SandBox()
	{
		pushLayer(layer.get());
	}

	~SandBox() override = default;

private:
	std::unique_ptr<Cardia::Layer> layer = std::make_unique<LayerTest>("First Layer");
};


std::unique_ptr<Cardia::Application> Cardia::CreateApplication()
{
	return std::make_unique<SandBox>();
}
