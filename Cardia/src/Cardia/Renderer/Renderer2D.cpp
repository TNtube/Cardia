#include "cdpch.hpp"
#include "Cardia/Renderer/Renderer2D.hpp"

#include <map>

#include "Cardia/Renderer/RenderAPI.hpp"
#include "Cardia/Renderer/Shader.hpp"

#include <glm/ext/matrix_transform.hpp>
#include <memory>
#include <numeric>
#include <glm/gtc/type_ptr.hpp>


namespace Cardia
{

	static std::unique_ptr<Renderer2D::Stats> s_Stats;

	
	constexpr uint32_t maxTriangle = 10000;
	constexpr uint32_t maxVertices = maxTriangle * 3;
	constexpr uint32_t maxIndices = maxTriangle * 3;
	constexpr int maxTextureSlots = 32; // TODO: get it from RenderAPI
	
	struct Vertex
	{
		glm::vec3 position;
		glm::vec4 color;
		glm::vec2 textureCoord;
		float textureIndex;
		float tilingFactor;
	};

	struct MeshIndices
	{
		std::vector<uint32_t> indices;
	};

	class Batch
	{
	public:
		Batch(const glm::vec3& cameraPosition, Shader* shader, int32_t zIndex) : m_zIndex(zIndex),
			camPos(cameraPosition), m_Shader(shader)
		{
			vertexArray = VertexArray::create();

			std::unique_ptr<VertexBuffer> vbo;
			vbo = VertexBuffer::create(maxVertices * sizeof(Vertex));

			vbo->setLayout({
				{ShaderDataType::Float3, "a_Position"},
				{ShaderDataType::Float4, "a_Color"},
				{ShaderDataType::Float2, "a_TexPos"},
				{ShaderDataType::Float, "a_TexIndex"},
				{ShaderDataType::Float, "a_TilingFactor"}
			});

			vertexBuffer = vbo.get();
			vertexArray->setVertexBuffer(std::move(vbo));

			vertexBufferData = std::vector<Vertex>();
			indexBufferData = std::vector<MeshIndices>();

			std::unique_ptr<IndexBuffer> ibo = IndexBuffer::create(maxIndices);
			indexBuffer = ibo.get();
			vertexArray->setIndexBuffer(std::move(ibo));
			indexOffset = 0;

			uint32_t whiteColor = 0xffffffff;
			whiteTexture = Texture2D::create(1, 1, &whiteColor);

			std::array<int, maxTextureSlots> samplers {};
			for (int32_t i = 0; i < maxTextureSlots; ++i)
			{
				samplers[i] = i;
			}

			shader->bind();
			shader->setIntArray("u_Textures", samplers.data(), maxTextureSlots);

			// Always white tex at pos 0
			textureSlots[0] = whiteTexture.get();
			startBash();
		}

		void startBash()
		{
			vertexCount = 0;
			indexOffset = 0;
			vertexBufferData.clear();
			indexBufferData.clear();
			textureSlotIndex = 1;
		}

		
		void nextBatch()
		{
			render();
			startBash();
		}

		void render()
		{
			std::ranges::sort(indexBufferData, [this](const MeshIndices a, const MeshIndices b)
			{
				const auto lambda = [this](const glm::vec3 va, const uint32_t ib)
				{
					return va + vertexBufferData[ib].position;
				};
				const auto vertexA = std::accumulate(a.indices.begin(), a.indices.end(), glm::vec3(0), lambda) / 3.0f;
				const auto vertexB = std::accumulate(b.indices.begin(), b.indices.end(), glm::vec3(0), lambda) / 3.0f;
				return glm::distance(vertexA, camPos) >= glm::distance(vertexB, camPos);
			});

			std::vector<uint32_t> iboData;

			for (const auto& object: indexBufferData)
			{
				for (const auto index: object.indices)
				{
					iboData.push_back(index);
				}
			}

			vertexBuffer->setData(vertexBufferData.data(), static_cast<int>(vertexBufferData.size()) * sizeof(Vertex));
			indexBuffer->setData(iboData.data(), static_cast<int>(iboData.size()) * sizeof(uint32_t));

			m_Shader->bind();
			for (int i = 0; i < textureSlotIndex; ++i)
			{
				textureSlots[i]->bind(i);
			}

			RenderAPI::get().drawIndexed(vertexArray.get(), vertexCount);
			s_Stats->drawCalls++;
		}

		void addMesh(std::vector<Vertex>& vertices, MeshIndices& indices, const Texture2D *texture)
		{
			if (vertexCount >= maxIndices)
				nextBatch();

			float textureIndex = 0;
			for(int i = 1; i < textureSlotIndex; ++i) {
				if (texture && *textureSlots[i] == *texture) {
					textureIndex = static_cast<float>(i);
					break;
				}
			}

			if (textureIndex == 0 && texture) {
				if (textureSlotIndex >= maxTextureSlots)
					nextBatch();

				textureSlots[textureSlotIndex] = texture;
				textureIndex = static_cast<float>(textureSlotIndex);
				textureSlotIndex++;
			}
			for (auto& vertex : vertices)
			{
				vertex.textureIndex = textureIndex;
			}

			vertexBufferData.reserve( vertexBufferData.size() + vertices.size() );
			vertexBufferData.insert(vertexBufferData.end(), vertices.begin(), vertices.end());

			for (auto& index: indices.indices)
			{
				index += indexOffset;
			}

			indexBufferData.push_back(indices);

			indexOffset += vertices.size();
			vertexCount += 6;
			s_Stats->triangleCount += 2;
		}
		int32_t zIndex() const { return m_zIndex; }
	private:
		int32_t m_zIndex;
		glm::vec3 camPos {};

		std::unique_ptr<VertexArray> vertexArray;
		VertexBuffer* vertexBuffer = nullptr;
		IndexBuffer* indexBuffer = nullptr;
		Shader* m_Shader;
		std::unique_ptr<Texture2D> whiteTexture;

		std::vector<Vertex> vertexBufferData;
		uint32_t vertexCount = 0;

		std::vector<MeshIndices> indexBufferData;
		uint32_t indexOffset {};

		std::array<const Texture2D*, maxTextureSlots> textureSlots {};
		int textureSlotIndex = 1;
	};

	struct Renderer2DData
	{
		std::vector<Batch> batches;
		glm::vec3 cameraPosition {};
		std::unique_ptr<Shader> basicShader;
		glm::mat4 viewProjectionMatrix {};
	};

	static std::unique_ptr<Renderer2DData> s_Data {};

	void Renderer2D::init()
	{
		s_Data = std::make_unique<Renderer2DData>();
		s_Stats = std::make_unique<Renderer2D::Stats>();
		s_Data->basicShader = Shader::create({"assets/shaders/basic.vert", "assets/shaders/basic.frag"});
		s_Data->batches.clear();
	}

	void Renderer2D::quit()
	{
		s_Data.reset();
	}

	void Renderer2D::beginScene(Camera& camera, glm::mat4& transform)
	{
		s_Data->batches.clear();
		s_Data->batches.emplace_back(glm::vec3(transform[3]), s_Data->basicShader.get(), 0);
		s_Data->cameraPosition = glm::vec3(transform[3]);
		s_Data->basicShader->setMat4("u_ViewProjection", camera.getProjectionMatrix() * glm::inverse(transform));
		s_Data->viewProjectionMatrix = camera.getProjectionMatrix() * glm::inverse(transform);
		s_Stats->drawCalls = 0;
		s_Stats->triangleCount = 0;
	}

	void Renderer2D::beginScene(Camera& camera, const glm::vec3& position)
	{
		s_Data->batches.clear();
		s_Data->batches.emplace_back(position, s_Data->basicShader.get(), 0);
		s_Data->cameraPosition = position;
		s_Data->basicShader->setMat4("u_ViewProjection", camera.getViewProjectionMatrix());
		s_Data->viewProjectionMatrix = camera.getViewProjectionMatrix();
		s_Stats->drawCalls = 0;
		s_Stats->triangleCount = 0;
	}

	void Renderer2D::endScene()
	{
		for (auto& batch : s_Data->batches)
		{
			batch.render();
		}
	}

	Renderer2D::Stats& Renderer2D::getStats()
	{
		return *s_Stats;
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

	void Renderer2D::drawRect(const glm::mat4 &transform, const Texture2D *texture, const glm::vec4 &color, float tilingFactor, int32_t zIndex)
	{
		constexpr glm::vec2 texCoords[] {
			{0.0f, 0.0f},
			{1.0f, 0.0f},
			{1.0f, 1.0f},
			{0.0f, 1.0f}
		};

		constexpr glm::vec4 rectPositions[]
		{
			{ -0.5f, -0.5f, 0.0f, 1.0f },
			{  0.5f, -0.5f, 0.0f, 1.0f },
			{  0.5f,  0.5f, 0.0f, 1.0f },
			{ -0.5f,  0.5f, 0.0f, 1.0f },
		};

		std::vector<Vertex> vertices;
		for (int i = 0; i < 4; ++i)
		{
			auto vertex = Vertex();
			vertex.position = transform * rectPositions[i];
			vertex.color = color;
			vertex.textureCoord = texCoords[i];
			vertex.tilingFactor = tilingFactor;
			vertices.push_back(vertex);
		}

		MeshIndices indices;
		indices.indices = std::vector<uint32_t>({ 0, 1, 2, 2, 3, 0 });

		s_Data->batches[0].addMesh(vertices, indices, texture);
	}
}
