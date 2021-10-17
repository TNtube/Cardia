#include "cdpch.hpp"
#include "Cardia/Renderer/Renderer2D.hpp"
#include "Cardia/Renderer/RenderCommand.hpp"

#include <glm/ext/matrix_transform.hpp>

namespace Cardia
{
	struct Renderer2DData {
		std::unique_ptr<VertexArray> vertexArray;

		std::unique_ptr<Shader> basicShader;
		std::unique_ptr<Texture2D> whiteTexture;
	};

	static std::unique_ptr<Renderer2DData> s_Data;

	void Renderer2D::Init()
	{
		s_Data = std::make_unique<Renderer2DData>();

		s_Data->vertexArray = Cardia::VertexArray::create();

		float vertices[] = {
			0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
			1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
			1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
			0.0f, 1.0f, 0.0f, 0.0f, 1.0f
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
		s_Data->vertexArray->addVertexBuffer(std::move(vertexBuffer));

		std::unique_ptr<Cardia::IndexBuffer> indexBuffer;
		indexBuffer = Cardia::IndexBuffer::create(indices, sizeof(indices) / sizeof(uint32_t));
		s_Data->vertexArray->setIndexBuffer(std::move(indexBuffer));

		s_Data->basicShader = Shader::create({"assets/basic.vert", "assets/basic.frag"});

		uint32_t whiteColor = 0xffffff;
		s_Data->whiteTexture = Texture2D::create(1, 1, &whiteColor);

		s_Data->basicShader->unbind();
		s_Data->vertexArray->unbind();

	}

	void Renderer2D::Quit()
	{
		s_Data.reset();
	}

	void Renderer2D::beginScene(OrthographicCamera camera)
	{
		s_Data->basicShader->bind();
		s_Data->basicShader->setMat4("u_ViewProjection", camera.getViewProjectionMatrix());
	}

	void Renderer2D::endScene()
	{

	}

	void Renderer2D::drawRect(const glm::vec3& position, const glm::vec2& size, const glm::vec4 &color)
	{
		drawRect(position, size, s_Data->whiteTexture.get(), color);
	}

	void Renderer2D::drawRect(const glm::vec3& position, const glm::vec2& size, const Texture* texture)
	{
		drawRect(position, size, texture, glm::vec4(1.0f));
	}

	void Renderer2D::drawRect(const glm::vec3& position, const glm::vec2& size, const Texture* texture, const glm::vec4 &color)
	{
		s_Data->basicShader->bind();
		s_Data->basicShader->setFloat4("u_Color", color);
		s_Data->basicShader->setInt("u_Texture", 0);

		auto transform = glm::translate(glm::mat4(1.0f), position)
				* glm::scale(glm::mat4(1.0f), {size.x, size.y, 0.0f});

		s_Data->basicShader->setMat4("u_Model", transform);


		texture->bind();

		s_Data->vertexArray->bind();
		RenderCommand::drawIndexed(s_Data->vertexArray.get());

	}
}