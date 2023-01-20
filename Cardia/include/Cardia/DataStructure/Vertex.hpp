#pragma once

#include <glm/glm.hpp>


namespace Cardia
{
	struct Vertex
	{
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec4 color;
		glm::vec2 textureCoord;
		float textureIndex;
		float tilingFactor;
		float entityID;

		/*
                glm::vec3 position;
                glm::vec3 normal;
                glm::vec4 color;
                glm::vec2 textureCoord;
                float textureIndex;
                float tilingFactor;
                float entityID;
		*/
	};
}