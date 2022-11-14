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
        };
        
        struct Mesh
        {
                std::vector<Vertex> vertices;
                std::vector<uint32_t> indices;
        };
}
