#include "Cardia/DataStructure/SubMesh.hpp"

namespace Cardia
{
	std::vector<VkVertexInputBindingDescription> SubMesh::Vertex::GetBindingDescriptions()
	{
		std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
		bindingDescriptions[0].binding = 0;
		bindingDescriptions[0].stride = sizeof(Vertex);
		bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return bindingDescriptions;
	}

	std::vector<VkVertexInputAttributeDescription> SubMesh::Vertex::GetAttributeDescriptions()
	{
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions;

		auto& position = attributeDescriptions.emplace_back();
		position.binding = 0;
		position.location = 0;
		position.format = VK_FORMAT_R32G32B32_SFLOAT;
		position.offset = offsetof(Vertex, Position);

		
		auto& color = attributeDescriptions.emplace_back();

		color.binding = 0;
		color.location = 1;
		color.format = VK_FORMAT_R32G32B32_SFLOAT;
		color.offset = offsetof(Vertex, Color);

		auto& normal = attributeDescriptions.emplace_back();

		normal.binding = 0;
		normal.location = 2;
		normal.format = VK_FORMAT_R32G32B32_SFLOAT;
		normal.offset = offsetof(Vertex, Normal);

		auto& textureCoord = attributeDescriptions.emplace_back();

		textureCoord.binding = 0;
		textureCoord.location = 3;
		textureCoord.format = VK_FORMAT_R32G32_SFLOAT;
		textureCoord.offset = offsetof(Vertex, TextureCoord);

		return attributeDescriptions;
	}
}
