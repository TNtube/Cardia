#pragma once
#include <glm/vec3.hpp>

#include "Cardia/DataStructure/Mesh.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "VertexArray.hpp"

namespace Cardia
{

	constexpr uint32_t maxTriangle = 100000;
	constexpr uint32_t maxVertices = maxTriangle * 3;
	constexpr uint32_t maxIndices = maxTriangle * 3;
	constexpr int maxTextureSlots = 32; // TODO: get it from RenderAPI

	struct BatchSpecification
	{
		int32_t layer;
		bool alpha;
		std::string shader;
		std::string texture;
		bool operator==(const BatchSpecification& other) const
		{
			return other.layer == layer
			       && other.alpha == alpha
			       && other.shader == shader
			       && other.texture == texture;
		}
	};
	
        class Batch
	{
	public:
		Batch(VertexArray* va, const glm::vec3& cameraPosition, const Texture2D* texture, const BatchSpecification& specification);
		void startBash();
		void render(bool alpha = false);
		bool addMesh(SubMesh* mesh);
        	BatchSpecification specification;
	private:
		glm::vec3 camPos {};

		VertexArray* vertexArray;
		VertexBuffer* vertexBuffer = nullptr;
		IndexBuffer* indexBuffer = nullptr;
		Shader* m_Shader;
		std::unique_ptr<Texture2D> whiteTexture;

		std::vector<Vertex> vertexBufferData;
		uint32_t indexCount = 0;

		std::vector<std::vector<uint32_t>> indexBufferData;
		uint32_t indexOffset {};
		const Texture2D* m_CurrentTexture = nullptr;
	};
}
