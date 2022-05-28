#include "cdpch.hpp"

#include "Cardia/Renderer/Renderer2D.hpp"
#include "Cardia/Renderer/RenderAPI.hpp"
#include "Cardia/Renderer/Shader.hpp"
#include "Cardia/Renderer/Batch.hpp"

#include <glm/ext/matrix_transform.hpp>
#include <memory>
#include <glm/gtc/type_ptr.hpp>


namespace Cardia
{

	static std::unique_ptr<Renderer2D::Stats> s_Stats;

	struct Renderer2DData
	{
		std::vector<Batch> batches;
		glm::vec3 cameraPosition {};
		std::unique_ptr<Shader> basicShader;
		glm::mat4 viewProjectionMatrix {};

		std::unique_ptr<VertexArray> vertexArray;
		
	};

	static std::unique_ptr<Renderer2DData> s_Data {};

	void Renderer2D::init()
	{
		s_Data = std::make_unique<Renderer2DData>();
		s_Stats = std::make_unique<Renderer2D::Stats>();
		s_Data->basicShader = Shader::create({"assets/shaders/basic.vert", "assets/shaders/basic.frag"});
		s_Data->batches.clear();
		s_Data->vertexArray = VertexArray::create();

		std::unique_ptr<VertexBuffer> vbo = VertexBuffer::create(maxVertices * sizeof(Vertex));

		vbo->setLayout({
			{ShaderDataType::Float3, "a_Position"},
			{ShaderDataType::Float4, "a_Color"},
			{ShaderDataType::Float2, "a_TexPos"},
			{ShaderDataType::Float, "a_TexIndex"},
			{ShaderDataType::Float, "a_TilingFactor"}
		});

		s_Data->vertexArray->setVertexBuffer(std::move(vbo));

		std::unique_ptr<IndexBuffer> ibo = IndexBuffer::create(maxIndices);
		s_Data->vertexArray->setIndexBuffer(std::move(ibo));
		
	}

	void Renderer2D::quit()
	{
		s_Data.reset();
	}

	void Renderer2D::beginScene(Camera& camera, glm::mat4& transform)
	{
		s_Data->batches.clear();
		s_Data->cameraPosition = glm::vec3(transform[3]);
		s_Data->basicShader->setMat4("u_ViewProjection", camera.getProjectionMatrix() * glm::inverse(transform));
		s_Data->viewProjectionMatrix = camera.getProjectionMatrix() * glm::inverse(transform);
		s_Stats->drawCalls = 0;
		s_Stats->triangleCount = 0;
	}

	void Renderer2D::beginScene(Camera& camera, const glm::vec3& position)
	{
		s_Data->batches.clear();
		s_Data->cameraPosition = position;
		s_Data->basicShader->setMat4("u_ViewProjection", camera.getViewProjectionMatrix());
		s_Data->viewProjectionMatrix = camera.getViewProjectionMatrix();
		s_Stats->drawCalls = 0;
		s_Stats->triangleCount = 0;
	}

	void Renderer2D::endScene()
	{
		std::ranges::sort(s_Data->batches, [](const Batch& a, const Batch& b)
		{
			return static_cast<float>(a.specification.layer) + static_cast<float>(a.specification.alpha) / 2.0f < static_cast<float>(b.specification.layer) + static_cast<float>(b.specification.alpha) / 2.0f;
		});
		int32_t lastLayer = 0;
		for (auto& batch : s_Data->batches)
		{
			batch.render(batch.specification.alpha);
			s_Stats->drawCalls++;
			if (lastLayer != batch.specification.layer)
			{
				RenderAPI::get().clearDepthBuffer();
			}
			lastLayer = batch.specification.layer;
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


		Mesh mesh;
		for (int i = 0; i < 4; ++i)
		{
			auto vertex = Vertex();
			vertex.position = transform * rectPositions[i];
			vertex.color = color;
			vertex.textureCoord = texCoords[i];
			vertex.tilingFactor = tilingFactor;
			mesh.vertices.push_back(vertex);
		}

		mesh.indices = std::vector<uint32_t>({ 0, 1, 2, 2, 3, 0 });
		s_Stats->triangleCount += 2;

		BatchSpecification specification;
		specification.alpha = color.a < 1.0f;
		specification.layer = zIndex;
		specification.shader = "basic";

		for (auto& batch : s_Data->batches)
		{
			if (batch.specification == specification && batch.addMesh(mesh, texture))
				return;
		}
		auto& batch = s_Data->batches.emplace_back(s_Data->vertexArray.get(), s_Data->cameraPosition, specification);
		batch.addMesh(mesh, texture);
	}
}
