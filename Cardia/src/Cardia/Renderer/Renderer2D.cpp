#include "cdpch.hpp"

#include "Cardia/Renderer/Renderer2D.hpp"
#include "Cardia/Renderer/Batch.hpp"

#include <memory>

namespace Cardia
{

	static std::unique_ptr<Renderer2D::Stats> s_Stats;

	struct LightData
	{
		Vector4f positionAndType {};
		Vector4f directionAndRange {};
		Vector4f colorAndCutOff {};
	};

	struct Renderer2DData
	{
		std::vector<Batch> batches;
		Vector3f cameraPosition {};
		Matrix4f viewProjectionMatrix {};

		// std::unique_ptr<VertexArray> vertexArray;
		// std::unique_ptr<StorageBuffer> lightStorageBuffer;
		std::vector<LightData> lightDataBuffer;

	};

	static std::unique_ptr<Renderer2DData> s_Data {};

	void Renderer2D::init()
	{
		return;
		s_Data = std::make_unique<Renderer2DData>();
		s_Stats = std::make_unique<Renderer2D::Stats>();
		s_Data->batches.clear();
		s_Data->lightDataBuffer.clear();
		// s_Data->vertexArray = VertexArray::create();
		//
		// std::unique_ptr<VertexBuffer> vbo = VertexBuffer::create(maxVertices/* * sizeof(Vertex)*/);
		//
		// vbo->setLayout({
		// 	{ShaderDataType::Float3, "a_Position"},
		// 	{ShaderDataType::Float3, "a_Normal"},
		// 	{ShaderDataType::Float4, "a_Color"},
		// 	{ShaderDataType::Float2, "a_TexPos"},
		// 	{ShaderDataType::Float, "a_TilingFactor"},
		// 	{ShaderDataType::Float, "a_EntityID"}
		// });
		//
		// // s_Data->vertexArray->setVertexBuffer(std::move(vbo));
		//
		// std::unique_ptr<IndexBuffer> ibo = IndexBuffer::create(maxIndices);
		// s_Data->vertexArray->setIndexBuffer(std::move(ibo));
	}

	void Renderer2D::quit()
	{
		s_Data.reset();
	}

	void Renderer2D::beginScene(Camera& camera, const Matrix4f& transform)
	{
		return;
		// s_Data->batches.clear();
		// s_Data->lightDataBuffer.clear();
		// s_Data->cameraPosition = Vector3f(transform[3]);
		// s_Data->basicShader->setMat4("u_ViewProjection", camera.GetProjectionMatrix() * glm::inverse(transform));
		// s_Data->basicShader->setFloat3("u_ViewPosition", s_Data->cameraPosition);
		// s_Data->viewProjectionMatrix = camera.GetProjectionMatrix() * glm::inverse(transform);
		// s_Stats->drawCalls = 0;
		// s_Stats->triangleCount = 0;
	}

	void Renderer2D::endScene()
	{
		return;
		// s_Data->lightStorageBuffer = StorageBuffer::create(s_Data->lightDataBuffer.size() * sizeof(LightData));
		// s_Data->lightStorageBuffer->setData(s_Data->lightDataBuffer.data(), s_Data->lightDataBuffer.size() * sizeof(LightData));
		//
		// std::ranges::sort(s_Data->batches, [](const Batch& a, const Batch& b)
		// {
		// 	return static_cast<float>(a.specification.layer) + static_cast<float>(a.specification.alpha) / 2.0f < static_cast<float>(b.specification.layer) + static_cast<float>(b.specification.alpha) / 2.0f;
		// });
		// s_Data->lightStorageBuffer->bind(0);
		std::string lastLayer;
		for (auto& batch : s_Data->batches)
		{
			if (lastLayer != std::to_string(batch.specification.layer))
			{
				// RenderAPI::get().clearDepthBuffer();
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

	void Renderer2D::drawRect(const Vector3f& position, const Vector2f& size, const Vector4f &color)
	{
		drawRect(position, size, nullptr, color);
	}

	void Renderer2D::drawRect(const Vector3f &position, const Vector2f &size, float rotation, const Vector4f &color)
	{
		drawRect(position, size, rotation, nullptr, color);
	}

	void Renderer2D::drawRect(const Vector3f& position, const Vector2f& size, const Texture* texture, float tilingFactor)
	{
		drawRect(position, size, texture, Vector4f(1.0f), tilingFactor);
	}

	void Renderer2D::drawRect(const Vector3f &position, const Vector2f &size, float rotation, const Texture *texture, float tilingFactor)
	{
		drawRect(position, size, rotation, texture, Vector4f(1.0f), tilingFactor);
	}

	void Renderer2D::drawRect(const Vector3f& position, const Vector2f& size, const Texture* texture, const Vector4f &color, float tilingFactor)
	{
		const Matrix4f transform = Matrix4f::Identity().Translate(position)
			* Matrix4f::Identity().Scale({ size.x, size.y, 1.0f });
		drawRect(transform, texture, color, tilingFactor);
	}

	void Renderer2D::drawRect(const Vector3f &position, const Vector2f &size, float rotation, const Texture *texture, const Vector4f &color, float tilingFactor)
	{
		const Matrix4f transform = Matrix4f::Identity().Translate(position)
			* Matrix4f::Identity().Rotate(Radianf::FromDegree(rotation), { 0.0f, 0.0f, 1.0f })
			* Matrix4f::Identity().Scale({ size.x, size.y, 1.0f });
		drawRect(transform, texture, color, tilingFactor);
	}

	void Renderer2D::drawRect(const Matrix4f &transform, const Vector4f &color)
	{
		drawRect(transform, nullptr, color);
	}

	void Renderer2D::drawRect(const Matrix4f &transform, const Texture *texture, const Vector4f &color, float tilingFactor, int32_t zIndex, float entityID)
	{
		constexpr Vector2f texCoords[] {
			{ 0.0f, 0.0f },
			{ 1.0f, 0.0f },
			{ 1.0f, 1.0f },
			{ 0.0f, 1.0f }
		};

		constexpr Vector4f rectPositions[]
		{
			{ -0.5f, -0.5f, 0.0f, 1.0f },
			{  0.5f, -0.5f, 0.0f, 1.0f },
			{  0.5f,  0.5f, 0.0f, 1.0f },
			{ -0.5f,  0.5f, 0.0f, 1.0f },
		};
		constexpr Vector4f normal { 0.0f, 0.0f, 1.0f, 0.0f };

		Mesh mesh;
		// const auto finalNormal = glm::mat3(glm::transpose(glm::inverse(transform))) * normal;
		for (int i = 0; i < sizeof(rectPositions) / sizeof(Vector4f); ++i)
		{
			// auto vertex = Vertex();
			// vertex.position = transform * rectPositions[i];
			// vertex.normal = finalNormal;
			// vertex.color = color;
			// vertex.textureCoord = texCoords[i % 4];
			// vertex.tilingFactor = tilingFactor;
			// vertex.entityID = entityID;
			// mesh.GetVertices().push_back(vertex);
		}

		mesh.GetIndices() = std::vector<uint32_t>({ 0, 1, 2, 2, 3, 0 });
		s_Stats->triangleCount += 2;

		BatchSpecification specification;
		specification.alpha = color.a < 1.0f;
		specification.layer = zIndex;
		specification.shader = "basic";
		// if (texture)
		// 	specification.texture = texture->GetPath();

		for (auto& batch : s_Data->batches)
		{
			if (batch.specification == specification && batch.addMesh(&mesh))
				return;
		}
		auto& batch = s_Data->batches.emplace_back(s_Data->cameraPosition, texture, specification);
		batch.addMesh(&mesh);
	}

	void Renderer2D::addLight(const Component::Transform& transform, const Component::Light& lightComponent)
	{
		auto& light = s_Data->lightDataBuffer.emplace_back();

		// light.positionAndType = Vector4f(transform.position, static_cast<uint32_t>(lightComponent.lightType));
		//
		// const auto forward = glm::rotate({transform.rotation}, Vector3f(0, -1, 0));
		// light.directionAndRange = Vector4f(forward, lightComponent.range);
		// light.colorAndCutOff = Vector4f(lightComponent.color, 1.0f - std::fmod(lightComponent.angle, 360.0f) / 360);
	}
}
