#include "Cardia.hpp"
#include <imgui.h>


class LayerTest : public Cardia::Layer
{
public:
	explicit LayerTest(std::string&& name)
		: Layer(name), m_Camera(-1.6f, 1.6f, -0.9f, 0.9f), m_CameraPosition(0.0f)
	{
		m_VertexArray.reset(Cardia::VertexArray::create());

		float vertices[] = {
			-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
			0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
			-0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f
		};

		uint32_t indices[] {
			0, 1, 2,
			2, 3, 0
		};

		std::unique_ptr<Cardia::VertexBuffer> vertexBuffer;
		vertexBuffer.reset(Cardia::VertexBuffer::create(vertices, sizeof(vertices)));


		Cardia::BufferLayout layout = {
			{Cardia::ShaderDataType::Float3, "position"},
			{Cardia::ShaderDataType::Float4, "col"}
		};

		vertexBuffer->setLayout(layout);
		m_VertexArray->addVertexBuffer(std::move(vertexBuffer));

		std::unique_ptr<Cardia::IndexBuffer> indexBuffer;
		indexBuffer.reset(Cardia::IndexBuffer::create(indices, sizeof(indices) / sizeof(uint32_t)));
		m_VertexArray->setIndexBuffer(std::move(indexBuffer));

		std::string vertexSrc = R"(
			#version 440 core

			layout(location = 0) in vec3 position;
			layout(location = 1) in vec4 col;

			out vec3 o_Pos;
			out vec4 o_Col;

			uniform mat4 u_ViewProjection;

			void main() {
				o_Pos = position;
				o_Col = col;
				gl_Position = u_ViewProjection * vec4(position, 1.0f);
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

		m_Shader.reset(Cardia::Shader::create(vertexSrc, fragmentSrc));
		m_Shader->unbind();
		m_VertexArray->unbind();
	}

	void onUpdate() override
	{
		if (Cardia::Input::isKeyPressed(Cardia::Key::Left))
			m_CameraPosition.x -= m_CameraSpeed;
		else if (Cardia::Input::isKeyPressed(Cardia::Key::Right))
			m_CameraPosition.x += m_CameraSpeed;

		if (Cardia::Input::isKeyPressed(Cardia::Key::Down))
			m_CameraPosition.y -= m_CameraSpeed;
		else if (Cardia::Input::isKeyPressed(Cardia::Key::Up))
			m_CameraPosition.y += m_CameraSpeed;

		m_Camera.setPosition(m_CameraPosition);

		Cardia::RenderCommand::setClearColor({0.2f, 0.2f, 0.2f, 1});
		Cardia::RenderCommand::clear();

		Cardia::Renderer::beginScene(m_Camera);

		Cardia::Renderer::submit(m_VertexArray, m_Shader);

		Cardia::Renderer::endScene();
	}

	void onEvent(Cardia::Event& event) override
	{
	}

	void onImGuiDraw() override
	{
	}

private:
	std::unique_ptr<Cardia::Shader> m_Shader;
	std::unique_ptr<Cardia::VertexArray> m_VertexArray;

	Cardia::OrthographicCamera m_Camera;

	glm::vec3 m_CameraPosition;
	float m_CameraSpeed = 0.1f;
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
