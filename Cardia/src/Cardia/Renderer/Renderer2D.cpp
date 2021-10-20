#include "cdpch.hpp"
#include "Cardia/Renderer/Renderer2D.hpp"
#include "Cardia/Renderer/RenderCommand.hpp"

#include <glm/ext/matrix_transform.hpp>

namespace Cardia
{
	struct RectVertex
	{
		glm::vec3 position;
		glm::vec4 color;
		glm::vec2 textureCoord;
	};

	struct Renderer2DData {
		const int maxRect = 10000;
		const int maxVertices = maxRect * 4;
		const int maxIndices = maxRect * 6;
		static const int maxTextureSlots = 32; // TODO: get it from RenderCommand

		std::unique_ptr<VertexArray> rectVertexArray;
		VertexBuffer* rectVertexBuffer = nullptr;
		std::unique_ptr<Shader> basicShader;
		std::unique_ptr<Texture2D> whiteTexture;

		uint32_t rectIndexCount = 0;
		std::unique_ptr<RectVertex> rectVertexBufferBase = nullptr;
		RectVertex* rectVertexBufferPtr = nullptr;

		std::array<Texture2D*, maxTextureSlots> textureSlots {};
		uint32_t textureSlotIndex = 1;

		int drawCalls = 0;
	};

	static std::unique_ptr<Renderer2DData> s_Data;

	void Renderer2D::Init()
	{
		s_Data = std::make_unique<Renderer2DData>();

		s_Data->rectVertexArray = VertexArray::create();

		std::unique_ptr<VertexBuffer> rectVBO;
		rectVBO = VertexBuffer::create(s_Data->maxVertices * sizeof(RectVertex));

		rectVBO->setLayout({
			{ShaderDataType::Float3, "a_Position"},
			{ShaderDataType::Float4, "a_Color"},
			{ShaderDataType::Float2, "a_TextPos"}
		});
		s_Data->rectVertexBuffer = rectVBO.get();
		s_Data->rectVertexArray->addVertexBuffer(std::move(rectVBO));

		s_Data->rectVertexBufferBase.reset(new RectVertex[s_Data->maxVertices]);

		std::unique_ptr<uint32_t[]> rectIndices(new uint32_t[s_Data->maxIndices]);

		uint32_t offset = 0;
		for(uint32_t i = 0; i < s_Data->maxIndices; i += 6) {
			rectIndices[i + 0] = offset + 0;
			rectIndices[i + 1] = offset + 1;
			rectIndices[i + 2] = offset + 2;
			rectIndices[i + 3] = offset + 2;
			rectIndices[i + 4] = offset + 3;
			rectIndices[i + 5] = offset + 0;
			offset += 4;
		}

		std::unique_ptr<IndexBuffer> rectIBO;
		rectIBO = IndexBuffer::create(rectIndices.get(), s_Data->maxIndices);
		s_Data->rectVertexArray->setIndexBuffer(std::move(rectIBO));


		s_Data->basicShader = Shader::create({"assets/basic.vert", "assets/basic.frag"});

		uint32_t whiteColor = 0xffffff;
		s_Data->whiteTexture = Texture2D::create(1, 1, &whiteColor);

		s_Data->basicShader->bind();
		s_Data->basicShader->setInt("u_Texture", 0);

		// Always white tex at pos 0
		s_Data->textureSlots[0] = s_Data->whiteTexture.get();
		s_Data->whiteTexture->bind();
	}

	void Renderer2D::Quit()
	{
		s_Data.reset();
	}

	void Renderer2D::beginScene(OrthographicCamera camera)
	{
		s_Data->basicShader->bind();
		s_Data->basicShader->setMat4("u_ViewProjection", camera.getViewProjectionMatrix());

		s_Data->drawCalls = 0;
		s_Data->rectIndexCount = 0;
		s_Data->rectVertexBufferPtr = s_Data->rectVertexBufferBase.get();
		s_Data->textureSlotIndex = 1;
	}

	void Renderer2D::endScene()
	{
		uint32_t dataSize = (uint8_t*)s_Data->rectVertexBufferPtr - (uint8_t*)s_Data->rectVertexBufferBase.get();
		s_Data->rectVertexBuffer->setData(s_Data->rectVertexBufferBase.get(), dataSize);
		render();
		Log::coreTrace("Number of draw calls per frames : {0}", s_Data->drawCalls);
	}

	void Renderer2D::render()
	{
		RenderCommand::drawIndexed(s_Data->rectVertexArray.get(), s_Data->rectIndexCount);
		s_Data->drawCalls++;
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
		s_Data->rectVertexBufferPtr->position = position;
		s_Data->rectVertexBufferPtr->color = color;
		s_Data->rectVertexBufferPtr->textureCoord = {0.0f, 0.0f};
		s_Data->rectVertexBufferPtr++;

		s_Data->rectVertexBufferPtr->position = {position.x + size.x, position.y, 0.0f};
		s_Data->rectVertexBufferPtr->color = color;
		s_Data->rectVertexBufferPtr->textureCoord = {1.0f, 0.0f};
		s_Data->rectVertexBufferPtr++;

		s_Data->rectVertexBufferPtr->position = {position.x + size.x, position.y + size.y, 0.0f};
		s_Data->rectVertexBufferPtr->color = color;
		s_Data->rectVertexBufferPtr->textureCoord = {1.0f, 1.0f};
		s_Data->rectVertexBufferPtr++;


		s_Data->rectVertexBufferPtr->position = {position.x, position.y + size.y, 0.0f};
		s_Data->rectVertexBufferPtr->color = color;
		s_Data->rectVertexBufferPtr->textureCoord = {0.0f, 1.0f};
		s_Data->rectVertexBufferPtr++;

		s_Data->rectIndexCount += 6;

	}
}