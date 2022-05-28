#pragma once
#include <glm/vec3.hpp>

#include "Mesh.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "VertexArray.hpp"

namespace Cardia
{

	constexpr uint32_t maxTriangle = 10000;
	constexpr uint32_t maxVertices = maxTriangle * 3;
	constexpr uint32_t maxIndices = maxTriangle * 3;
	constexpr int maxTextureSlots = 32; // TODO: get it from RenderAPI

	
        class Batch
	{
	public:
		Batch(VertexArray* va, const glm::vec3& cameraPosition, Shader* shader, int32_t zIndex);
		void startBash();
		void render();
		bool addMesh(Mesh& mesh, const Texture2D *texture);
        	
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

		std::vector<std::vector<uint32_t>> indexBufferData;
		uint32_t indexOffset {};

		std::array<const Texture2D*, maxTextureSlots> textureSlots {};
		int textureSlotIndex = 1;
	};
}
