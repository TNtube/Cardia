#include "cdpch.hpp"
#include "Cardia/Renderer/Renderer2D.hpp"
#include "Cardia/Renderer/RenderCommand.hpp"

#include <glm/ext/matrix_transform.hpp>
#include <memory>

namespace Cardia
{
	struct RectVertex
	{
		glm::vec3 position;
		glm::vec4 color;
		glm::vec2 textureCoord;
		float textureIndex;
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
		std::unique_ptr<RectVertex[]> rectVertexBufferBase = nullptr;
		RectVertex* rectVertexBufferPtr = nullptr;

		std::array<const Texture2D*, maxTextureSlots> textureSlots {};
		int textureSlotIndex = 1;

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
			{ShaderDataType::Float2, "a_TexPos"},
			{ShaderDataType::Int, "a_TexIndex"}
		});

		s_Data->rectVertexBuffer = rectVBO.get();
		s_Data->rectVertexArray->addVertexBuffer(std::move(rectVBO));

		s_Data->rectVertexBufferBase = std::make_unique<RectVertex[]>(s_Data->maxVertices);

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


		s_Data->basicShader = Shader::create({"assets/shaders/basic.vert", "assets/shaders/basic.frag"});

		uint32_t whiteColor = 0xffffff;
		s_Data->whiteTexture = Texture2D::create(1, 1, &whiteColor);

		int32_t samplers[Renderer2DData::maxTextureSlots];
		for (int32_t i = 0; i < Renderer2DData::maxTextureSlots; ++i)
		{
			samplers[i] = i;
		}

		s_Data->basicShader->bind();
		s_Data->basicShader->setIntArray("u_Textures", samplers, Renderer2DData::maxTextureSlots);

		// Always white tex at pos 0
		s_Data->textureSlots[0] = s_Data->whiteTexture.get();
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
		render();
	}

	void Renderer2D::render()
	{

		uint32_t dataSize = (uint8_t*)s_Data->rectVertexBufferPtr - (uint8_t*)s_Data->rectVertexBufferBase.get();
		s_Data->rectVertexBuffer->setData(s_Data->rectVertexBufferBase.get(), dataSize);

		s_Data->basicShader->bind();
		for (int i = 0; i < s_Data->textureSlotIndex; ++i)
		{
			s_Data->textureSlots[i]->bind(i);
			// Log::coreInfo("Texture index : {0}, Width : {1}", i, s_Data->textureSlots[i]->getWidth());
		}

		RenderCommand::drawIndexed(s_Data->rectVertexArray.get(), s_Data->rectIndexCount);
		s_Data->drawCalls++;
	}

	void Renderer2D::drawRect(const glm::vec3& position, const glm::vec2& size, const glm::vec4 &color)
	{
		drawRect(position, size, nullptr, color);
	}

	void Renderer2D::drawRect(const glm::vec3& position, const glm::vec2& size, const Texture2D* texture)
	{
		drawRect(position, size, texture, glm::vec4(1.0f));
	}

	void Renderer2D::drawRect(const glm::vec3& position, const glm::vec2& size, const Texture2D* texture, const glm::vec4 &color)
	{
		float textureIndex = 0;
		for(int i = 1; i < s_Data->textureSlotIndex; ++i) {
			if (texture && *s_Data->textureSlots[i] == *texture) {
				textureIndex = static_cast<float>(i);
				break;
			}
		}

		if (textureIndex == 0 && texture) {
			s_Data->textureSlots[s_Data->textureSlotIndex] = texture;
			textureIndex = static_cast<float>(s_Data->textureSlotIndex);
			s_Data->textureSlotIndex++;
		}

		s_Data->rectVertexBufferPtr->position = position;
		s_Data->rectVertexBufferPtr->color = color;
		s_Data->rectVertexBufferPtr->textureCoord = {0.0f, 0.0f};
		s_Data->rectVertexBufferPtr->textureIndex = textureIndex;
		s_Data->rectVertexBufferPtr++;

		s_Data->rectVertexBufferPtr->position = {position.x + size.x, position.y, 0.0f};
		s_Data->rectVertexBufferPtr->color = color;
		s_Data->rectVertexBufferPtr->textureCoord = {1.0f, 0.0f};
		s_Data->rectVertexBufferPtr->textureIndex = textureIndex;
		s_Data->rectVertexBufferPtr++;

		s_Data->rectVertexBufferPtr->position = {position.x + size.x, position.y + size.y, 0.0f};
		s_Data->rectVertexBufferPtr->color = color;
		s_Data->rectVertexBufferPtr->textureCoord = {1.0f, 1.0f};
		s_Data->rectVertexBufferPtr->textureIndex = textureIndex;
		s_Data->rectVertexBufferPtr++;

		s_Data->rectVertexBufferPtr->position = {position.x, position.y + size.y, 0.0f};
		s_Data->rectVertexBufferPtr->color = color;
		s_Data->rectVertexBufferPtr->textureCoord = {0.0f, 1.0f};
		s_Data->rectVertexBufferPtr->textureIndex = textureIndex;
		s_Data->rectVertexBufferPtr++;

		s_Data->rectIndexCount += 6;

	}
}