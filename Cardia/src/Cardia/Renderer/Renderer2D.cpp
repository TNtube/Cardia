#include "cdpch.hpp"

#include "Cardia/Renderer/Renderer2D.hpp"
#include "Cardia/Renderer/RenderAPI.hpp"
#include "Cardia/Renderer/Shader.hpp"
#include "Cardia/Renderer/Batch.hpp"

#include <glm/ext/matrix_transform.hpp>
#include <memory>
#include <glm/gtc/type_ptr.hpp>

#include <glm/gtx/quaternion.hpp>

namespace Cardia
{

	static std::unique_ptr<Renderer2D::Stats> s_Stats;

	struct LightData
	{
		glm::vec4 positionAndType {};
		glm::vec4 directionAndRange {};
		glm::vec4 colorAndCutOff {};
	};

	struct Renderer2DData
	{
		std::vector<Batch> batches;
		glm::vec3 cameraPosition {};
		std::unique_ptr<Shader> basicShader;
		glm::mat4 viewProjectionMatrix {};

		std::unique_ptr<VertexArray> vertexArray;
		std::unique_ptr<StorageBuffer> lightStorageBuffer;
		std::vector<LightData> lightDataBuffer;

	};

	static std::unique_ptr<Renderer2DData> s_Data {};

	void Renderer2D::init()
	{
		s_Data = std::make_unique<Renderer2DData>();
		s_Stats = std::make_unique<Renderer2D::Stats>();
		s_Data->basicShader = Shader::create({"resources/shaders/basic.vert", "resources/shaders/basic.frag"});
		s_Data->batches.clear();
		s_Data->lightDataBuffer.clear();
		s_Data->vertexArray = VertexArray::create();

		std::unique_ptr<VertexBuffer> vbo = VertexBuffer::create(maxVertices * sizeof(Vertex));

		vbo->setLayout({
			{ShaderDataType::Float3, "a_Position"},
			{ShaderDataType::Float3, "a_Normal"},
			{ShaderDataType::Float4, "a_Color"},
			{ShaderDataType::Float2, "a_TexPos"},
			{ShaderDataType::Float, "a_TilingFactor"},
			{ShaderDataType::Float, "a_EntityID"}
		});

		s_Data->vertexArray->setVertexBuffer(std::move(vbo));

		std::unique_ptr<IndexBuffer> ibo = IndexBuffer::create(maxIndices);
		s_Data->vertexArray->setIndexBuffer(std::move(ibo));
	}

	void Renderer2D::quit()
	{
		s_Data.reset();
	}

	void Renderer2D::beginScene(Camera& camera, const glm::mat4& transform)
	{
		s_Data->batches.clear();
		s_Data->lightDataBuffer.clear();
		s_Data->cameraPosition = glm::vec3(transform[3]);
		s_Data->basicShader->setMat4("u_ViewProjection", camera.getProjectionMatrix() * glm::inverse(transform));
		s_Data->basicShader->setFloat3("u_ViewPosition", s_Data->cameraPosition);
		s_Data->viewProjectionMatrix = camera.getProjectionMatrix() * glm::inverse(transform);
		s_Stats->drawCalls = 0;
		s_Stats->triangleCount = 0;
	}

	void Renderer2D::endScene()
	{
		s_Data->lightStorageBuffer = StorageBuffer::create(s_Data->lightDataBuffer.size() * sizeof(LightData));
		s_Data->lightStorageBuffer->setData(s_Data->lightDataBuffer.data(), s_Data->lightDataBuffer.size() * sizeof(LightData));

		std::ranges::sort(s_Data->batches, [](const Batch& a, const Batch& b)
		{
			return static_cast<float>(a.specification.layer) + static_cast<float>(a.specification.alpha) / 2.0f < static_cast<float>(b.specification.layer) + static_cast<float>(b.specification.alpha) / 2.0f;
		});
		s_Data->lightStorageBuffer->bind(0);
		std::string lastLayer;
		for (auto& batch : s_Data->batches)
		{
			if (lastLayer != std::to_string(batch.specification.layer))
			{
				RenderAPI::get().clearDepthBuffer();
			}
			batch.render(batch.specification.alpha);
			s_Stats->drawCalls++;
			lastLayer = std::to_string(batch.specification.layer);
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

	void Renderer2D::drawRect(const glm::mat4 &transform, const Texture2D *texture, const glm::vec4 &color, float tilingFactor, int32_t zIndex, float entityID)
	{
		constexpr glm::vec2 texCoords[] {
			{ 0.0f, 0.0f },
			{ 1.0f, 0.0f },
			{ 1.0f, 1.0f },
			{ 0.0f, 1.0f }
		};

		constexpr glm::vec4 rectPositions[]
		{
			{ -0.5f, -0.5f, 0.0f, 1.0f },
			{  0.5f, -0.5f, 0.0f, 1.0f },
			{  0.5f,  0.5f, 0.0f, 1.0f },
			{ -0.5f,  0.5f, 0.0f, 1.0f },
		};
		constexpr glm::vec4 normal { 0.0f, 0.0f, 1.0f, 0.0f };

		SubMesh mesh;
		const auto finalNormal = glm::mat3(glm::transpose(glm::inverse(transform))) * normal;
		for (int i = 0; i < sizeof(rectPositions) / sizeof(glm::vec4); ++i)
		{
			auto vertex = Vertex();
			vertex.position = transform * rectPositions[i];
			vertex.normal = finalNormal;
			vertex.color = color;
			vertex.textureCoord = texCoords[i % 4];
			vertex.tilingFactor = tilingFactor;
			vertex.entityID = entityID;
			mesh.GetVertices().push_back(vertex);
		}

		mesh.GetIndices() = std::vector<uint32_t>({ 0, 1, 2, 2, 3, 0 });
		s_Stats->triangleCount += 2;

		BatchSpecification specification;
		specification.alpha = color.a < 1.0f;
		specification.layer = zIndex;
		specification.shader = "basic";
		if (texture)
			specification.texture = texture->getPath();

		for (auto& batch : s_Data->batches)
		{
			if (batch.specification == specification && batch.addMesh(&mesh))
				return;
		}
		auto& batch = s_Data->batches.emplace_back(s_Data->vertexArray.get(), s_Data->cameraPosition, texture, specification);
		batch.addMesh(&mesh);
	}

	void Renderer2D::addLight(const Component::Transform& transform, const Component::Light& lightComponent)
	{
		auto& light = s_Data->lightDataBuffer.emplace_back();

		light.positionAndType = glm::vec4(transform.position, static_cast<uint32_t>(lightComponent.lightType));

		const auto forward = glm::rotate({transform.rotation}, glm::vec3(0, -1, 0));
		light.directionAndRange = glm::vec4(forward, lightComponent.range);
		light.colorAndCutOff = glm::vec4(lightComponent.color, 1.0f - std::fmod(lightComponent.angle, 360.0f) / 360);
	}
}
