#include "Cardia.hpp"
#include <imgui.h>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Platform/OpenGL/OpenGLShader.hpp"
#include <Cardia/ImGui/ImGuiExt.hpp>

class LayerTest : public Cardia::Layer
{
public:
	explicit LayerTest(std::string name)
		: Layer(std::move(name))
	{
		m_VertexArray = Cardia::VertexArray::create();

		float vertices[] = {
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // 1.0f, 0.0f, 0.0f, 1.0f,
			 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, // 0.0f, 1.0f, 0.0f, 1.0f,
			 0.5f,  0.5f, 0.0f, 1.0f, 1.0f, // 0.0f, 0.0f, 1.0f, 1.0f,
			-0.5f,  0.5f, 0.0f, 0.0f, 1.0f  // 1.0f, 0.0f, 1.0f, 1.0f
		};

		uint32_t indices[] {
			0, 1, 2,
			2, 3, 0
		};

		std::unique_ptr<Cardia::VertexBuffer> vertexBuffer;
		vertexBuffer = Cardia::VertexBuffer::create(vertices, sizeof(vertices));


		Cardia::BufferLayout layout = {
			{Cardia::ShaderDataType::Float3, "a_Position"},
			{Cardia::ShaderDataType::Float2, "a_TextPos"}
		};

		vertexBuffer->setLayout(layout);
		m_VertexArray->addVertexBuffer(std::move(vertexBuffer));

		std::unique_ptr<Cardia::IndexBuffer> indexBuffer;
		indexBuffer = Cardia::IndexBuffer::create(indices, sizeof(indices) / sizeof(uint32_t));
		m_VertexArray->setIndexBuffer(std::move(indexBuffer));

		m_Shader = Cardia::Shader::create({"assets/basic.vert", "assets/basic.frag"});
		m_Shader->unbind();
		m_VertexArray->unbind();

		m_TextureBox = Cardia::Texture2D::create("assets/container.jpg");
		m_TextureSquare = Cardia::Texture2D::create("assets/square.jpg");
	}

	void onUpdate(Cardia::DeltaTime deltaTime) override
	{
		float velocity = m_CameraSpeed * deltaTime.seconds();
		if (Cardia::Input::isKeyPressed(Cardia::Key::Left))
			m_CameraPosition.x -= velocity;
		else if (Cardia::Input::isKeyPressed(Cardia::Key::Right))
			m_CameraPosition.x += velocity;

		if (Cardia::Input::isKeyPressed(Cardia::Key::Down))
			m_CameraPosition.y -= velocity;
		else if (Cardia::Input::isKeyPressed(Cardia::Key::Up))
			m_CameraPosition.y += velocity;

		m_Camera.setPosition(m_CameraPosition);

		Cardia::RenderCommand::setClearColor({0.2f, 0.2f, 0.2f, 1});
		Cardia::RenderCommand::clear();

		Cardia::Renderer::beginScene(m_Camera);

		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(m_Scale));

		// Extremely temporary
		dynamic_cast<Cardia::OpenGLShader&>(*m_Shader).bind();
		dynamic_cast<Cardia::OpenGLShader&>(*m_Shader).setUniformInt("u_Texture", 0);

		m_TextureSquare->bind();
		for (int x = 0; x < 10; ++x)
		{
			for (int y = 0; y < 10; ++y)
			{
				glm::vec3 pos(static_cast<float>(x) * (m_Scale + m_Scale / 10), static_cast<float>(y) * (m_Scale + m_Scale / 10), 0.0f);
				glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;
				Cardia::Renderer::submit(m_VertexArray.get(), m_Shader.get(), transform);
			}
		}

		m_TextureBox->bind();
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f));
		Cardia::Renderer::submit(m_VertexArray.get(), m_Shader.get(), transform);

		Cardia::Renderer::endScene();
	}

	void onEvent(Cardia::Event& event) override
	{
		Cardia::EventDispatcher dispatcher(event);
		dispatcher.dispatch<Cardia::MouseScrolledEvent>([this](const Cardia::MouseScrolledEvent& ev) {
			m_Scale += ev.getOffSetY() / 100;
			return false;
		});
	}

	void onImGuiDraw(Cardia::DeltaTime deltaTime) override
	{
		ImGui::Begin("Debug tools");

		// Section: SandBox
		if (ImGui::CollapsingHeader("SandBox", ImGuiTreeNodeFlags_DefaultOpen)) {
			// Section: SandBox > Camera
			ImGui::Text("Camera");
			Cardia::ImGuiExt::InputVec3("Position", m_CameraPosition, 0.005f);
			ImGui::DragFloat("Speed", &m_CameraSpeed, 0.005f);
		}

		ImGui::End();
	}

private:
	std::unique_ptr<Cardia::Shader> m_Shader;
	std::unique_ptr<Cardia::VertexArray> m_VertexArray;

	std::unique_ptr<Cardia::Texture2D> m_TextureBox;
	std::unique_ptr<Cardia::Texture2D> m_TextureSquare;

	Cardia::OrthographicCamera m_Camera {-1.6f, 1.6f, -0.9f, 0.9f};

	glm::vec3 m_CameraPosition {0.0f};
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
