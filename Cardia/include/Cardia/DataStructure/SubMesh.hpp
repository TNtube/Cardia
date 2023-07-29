#pragma once

#include <vector>

#include <vulkan/vulkan.hpp>

#include "Cardia/Math/Vector2.hpp"
#include "Cardia/Math/Vector3.hpp"


namespace Cardia
{
	class SubMesh
	{
	public:
		
		struct Vertex
		{
			Vector3f Position;
			Vector3f Color;
			Vector3f Normal;
			Vector2f TextureCoord;

			/*
			glm::vec3 position;
			glm::vec3 normal;
			glm::vec4 color;
			glm::vec2 textureCoord;
			float tilingFactor; // no
			float entityID; // no
			*/

			static std::vector<VkVertexInputBindingDescription> GetBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();
		};

		
		SubMesh() = default;
		std::vector<Vertex>& GetVertices() { return  m_Vertices; }
		const std::vector<Vertex>& GetVertices() const { return  m_Vertices; }
		std::vector<uint32_t>& GetIndices() { return  m_Indices; }
		const std::vector<uint32_t>& GetIndices() const { return  m_Indices; }
		uint32_t& GetMaterialIndex() { return m_MaterialIndex; }
		uint32_t GetMaterialIndex() const { return m_MaterialIndex; }

	private:
		uint32_t m_MaterialIndex = 0;
		std::vector<Vertex> m_Vertices;
		std::vector<uint32_t> m_Indices;
	};
}
