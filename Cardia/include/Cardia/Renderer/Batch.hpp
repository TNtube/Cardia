#pragma once

#include "Cardia/DataStructure/Model.hpp"
#include "Texture.hpp"

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
		Batch(const Vector3f& cameraPosition, const Texture* texture, const BatchSpecification& specification);
		void startBash();
		void render(bool alpha = false);
		bool addMesh(Mesh* mesh);
		BatchSpecification specification;
	private:
		Vector3f camPos {};

		// VertexArray* vertexArray;
		// VertexBuffer* vertexBuffer = nullptr;
		// IndexBuffer* indexBuffer = nullptr;

		// std::vector<Vertex> vertexBufferData;
		uint32_t indexCount = 0;

		std::vector<std::vector<uint32_t>> indexBufferData;
		uint32_t indexOffset {};
		const Texture* m_CurrentTexture = nullptr;
	};
}
