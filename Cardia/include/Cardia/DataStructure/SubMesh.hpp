#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>


namespace Cardia
{
	class SubMesh
	{
	public:
		
		struct Vertex
		{
			glm::vec3 position;
			glm::vec3 color;
			glm::vec3 normal;
			glm::vec2 textureCoord;

			/*
			glm::vec3 position;
			glm::vec3 normal;
			glm::vec4 color;
			glm::vec2 textureCoord;
			float tilingFactor;
			float entityID;
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
