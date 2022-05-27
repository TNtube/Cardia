#pragma once
#include <glm/vec3.hpp>

#include "Shader.hpp"
#include "Texture.hpp"
#include "VertexArray.hpp"

namespace Cardia
{

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
		Batch(VertexArray* va, const glm::vec3& cameraPosition, Shader* shader, int32_t zIndex);
		void startBash();
		void render();
		bool addMesh(std::vector<Vertex>& vertices, MeshIndices& indices, const Texture2D *texture);
        	
		inline int32_t zIndex() const { return m_zIndex; }
	private:
		int32_t m_zIndex;
		glm::vec3 camPos {};

		VertexArray* vertexArray;
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
}
