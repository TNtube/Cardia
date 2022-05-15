#include "cdpch.hpp"
#include "Cardia/Renderer/Renderer2D.hpp"
#include "Cardia/Renderer/RenderAPI.hpp"
#include "Cardia/Renderer/Shader.hpp"

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
		float tilingFactor;
	};

	struct Rect
	{
		RectVertex vertices[4];
	};

	struct Renderer2DData {
		const uint32_t maxRect = 10000;
		const uint32_t maxVertices = maxRect * 4;
		const uint32_t maxIndices = maxRect * 6;
		static constexpr int maxTextureSlots = 32; // TODO: get it from RenderAPI

		glm::vec3 camPos;

		std::unique_ptr<VertexArray> rectVertexArray;
		VertexBuffer* rectVertexBuffer = nullptr;
		std::unique_ptr<Shader> basicShader;
		std::unique_ptr<Texture2D> whiteTexture;

		glm::vec4 rectPositions[4] {};
		uint32_t rectIndexCount = 0;
		std::unique_ptr<Rect[]> rectBufferBase = nullptr;
		Rect* rectBufferPtr = nullptr;

		std::array<const Texture2D*, maxTextureSlots> textureSlots {};
		int textureSlotIndex = 1;

		Renderer2D::Stats stats {};
	};

	static std::unique_ptr<Renderer2DData> s_Data;

	void Renderer2D::init()
	{
		s_Data = std::make_unique<Renderer2DData>();

		s_Data->rectVertexArray = VertexArray::create();

		std::unique_ptr<VertexBuffer> rectVBO;
		rectVBO = VertexBuffer::create(s_Data->maxVertices * sizeof(RectVertex));

		rectVBO->setLayout({
			{ShaderDataType::Float3, "a_Position"},
			{ShaderDataType::Float4, "a_Color"},
			{ShaderDataType::Float2, "a_TexPos"},
			{ShaderDataType::Float, "a_TexIndex"},
			{ShaderDataType::Float, "a_TilingFactor"}
		});

		s_Data->rectVertexBuffer = rectVBO.get();
		s_Data->rectVertexArray->addVertexBuffer(std::move(rectVBO));

		s_Data->rectBufferBase = std::make_unique<Rect[]>(s_Data->maxRect);

		std::unique_ptr<uint32_t[]> rectIndices = std::make_unique<uint32_t[]>(s_Data->maxIndices);

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

		std::unique_ptr<IndexBuffer> rectIBO = IndexBuffer::create(rectIndices.get(), s_Data->maxIndices);
		s_Data->rectVertexArray->setIndexBuffer(std::move(rectIBO));


		s_Data->basicShader = Shader::create({"assets/shaders/basic.vert", "assets/shaders/basic.frag"});

		uint32_t whiteColor = 0xffffffff;
		s_Data->whiteTexture = Texture2D::create(1, 1, &whiteColor);

		std::array<int, Renderer2DData::maxTextureSlots> samplers {};
		for (int32_t i = 0; i < Renderer2DData::maxTextureSlots; ++i)
		{
			samplers[i] = i;
		}

		s_Data->basicShader->bind();
		s_Data->basicShader->setIntArray("u_Textures", samplers.data(), Renderer2DData::maxTextureSlots);

		// Always white tex at pos 0
		s_Data->textureSlots[0] = s_Data->whiteTexture.get();

		s_Data->rectPositions[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		s_Data->rectPositions[1] = {  0.5f, -0.5f, 0.0f, 1.0f };
		s_Data->rectPositions[2] = {  0.5f,  0.5f, 0.0f, 1.0f };
		s_Data->rectPositions[3] = { -0.5f,  0.5f, 0.0f, 1.0f };
	}

	void Renderer2D::quit()
	{
		s_Data.reset();
	}

	void Renderer2D::beginScene(Camera& camera, glm::mat4& transform)
	{
		s_Data->basicShader->bind();
		s_Data->camPos = glm::vec3(transform[3]);
		s_Data->basicShader->setMat4("u_ViewProjection", camera.getProjectionMatrix() * glm::inverse(transform));
		s_Data->stats.drawCalls = 0;
		s_Data->stats.triangleCount = 0;
		startBash();
	}

	void Renderer2D::beginScene(Camera& camera, const glm::vec3& position)
	{
		s_Data->basicShader->bind();
		s_Data->camPos = position;
		s_Data->basicShader->setMat4("u_ViewProjection", camera.getViewProjectionMatrix());
		s_Data->stats.drawCalls = 0;
		s_Data->stats.triangleCount = 0;
		startBash();
	}


	void Renderer2D::startBash()
	{
		s_Data->rectIndexCount = 0;
		s_Data->rectBufferPtr = s_Data->rectBufferBase.get();
		s_Data->textureSlotIndex = 1;
	}

	void Renderer2D::nextBatch()
	{
		render();
		startBash();
	}

	void Renderer2D::endScene()
	{
		render();
	}

	void Renderer2D::render()
	{
		const auto dataSize = static_cast<uint32_t>(reinterpret_cast<uint8_t*>(s_Data->rectBufferPtr) -
							reinterpret_cast<uint8_t*>(s_Data->rectBufferBase.get()));

		
		/*std::sort(s_Data->rectBufferBase.get(), s_Data->rectBufferBase.get() + dataSize,
			[](Rect v1, Rect v2) {
				return v1.vertices[0].position.z < v2.vertices[0].position.z;
		});*/

		s_Data->rectVertexBuffer->setData(s_Data->rectBufferBase.get(), dataSize * 4);

		s_Data->basicShader->bind();
		for (int i = 0; i < s_Data->textureSlotIndex; ++i)
		{
			s_Data->textureSlots[i]->bind(i);
		}

		RenderAPI::get().drawIndexed(s_Data->rectVertexArray.get(), s_Data->rectIndexCount);
		s_Data->stats.drawCalls++;
	}

	Renderer2D::Stats& Renderer2D::getStats()
	{
		return s_Data->stats;
	}

	void Renderer2D::drawRect(const glm::vec3& position, const glm::vec2& size, const glm::vec4 &color)
	{
		drawRect(position, size, nullptr, color);
	}

	void Renderer2D::drawRect(const glm::vec3 &position, const glm::vec2 &size, float rotation, const glm::vec4 &color)
	{
		drawRect(position, size, rotation, nullptr, color);
	}

	void Renderer2D::drawRect(const glm::vec3& position, const glm::vec2& size, const Texture2D* texture, float tilingFactor)
	{
		drawRect(position, size, texture, glm::vec4(1.0f), tilingFactor);
	}

	void Renderer2D::drawRect(const glm::vec3 &position, const glm::vec2 &size, float rotation, const Texture2D *texture, float tilingFactor)
	{
		drawRect(position, size, rotation, texture, glm::vec4(1.0f), tilingFactor);
	}

	void Renderer2D::drawRect(const glm::vec3& position, const glm::vec2& size, const Texture2D* texture, const glm::vec4 &color, float tilingFactor)
	{
		const glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
				      * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });
		drawRect(transform, texture, color, tilingFactor);
	}

	void Renderer2D::drawRect(const glm::vec3 &position, const glm::vec2 &size, float rotation, const Texture2D *texture, const glm::vec4 &color, float tilingFactor)
	{
		const glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
				      * glm::rotate(glm::mat4(1.0f), glm::radians(rotation), {0.0f, 0.0f, 1.0f})
				      * glm::scale(glm::mat4(1.0f), {size.x, size.y, 1.0f});
		drawRect(transform, texture, color, tilingFactor);
	}

	void Renderer2D::drawRect(const glm::mat4 &transform, const glm::vec4 &color)
	{
		drawRect(transform, nullptr, color);
	}

	void Renderer2D::drawRect(const glm::mat4 &transform, const Texture2D *texture, const glm::vec4 &color, float tilingFactor)
	{
		if (s_Data->rectIndexCount >= s_Data->maxIndices)
			nextBatch();

		float textureIndex = 0;
		for(int i = 1; i < s_Data->textureSlotIndex; ++i) {
			if (texture && *s_Data->textureSlots[i] == *texture) {
				textureIndex = static_cast<float>(i);
				break;
			}
		}

		if (textureIndex == 0 && texture) {
			if (s_Data->textureSlotIndex >= Renderer2DData::maxTextureSlots)
				nextBatch();

			s_Data->textureSlots[s_Data->textureSlotIndex] = texture;
			textureIndex = static_cast<float>(s_Data->textureSlotIndex);
			s_Data->textureSlotIndex++;
		}

		constexpr glm::vec2 texCoords[] {
			{0.0f, 0.0f},
			{1.0f, 0.0f},
			{1.0f, 1.0f},
			{0.0f, 1.0f}
		};

		for (int i = 0; i < 4; ++i)
		{
			auto& vertex = s_Data->rectBufferPtr->vertices[i];
			vertex.position = transform * s_Data->rectPositions[i];
			vertex.color = color;
			vertex.textureCoord = texCoords[i];
			vertex.textureIndex = textureIndex;
			vertex.tilingFactor = tilingFactor;
		}
		s_Data->rectBufferPtr++;
		s_Data->rectIndexCount += 6;
		s_Data->stats.triangleCount += 2;
	}
}