#pragma once


#include <vulkan/vulkan.h>
#include "Cardia/Math/Vector3.hpp"
#include "Cardia/Math/Vector2.hpp"

namespace Cardia
{


	struct Vertex
	{
		Vector3f Position;
		Vector3f Normal;
		Vector2f TextureCoord;
		Vector3f Tangent;

		static std::vector<VkVertexInputBindingDescription> GetBindingDescriptions();
		static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();
	};
}