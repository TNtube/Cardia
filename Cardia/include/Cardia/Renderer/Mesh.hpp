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
        };
        
        struct Mesh
        {
		Mesh() = default;
		Mesh(std::vector<Vertex> verts, std::vector<uint32_t> inds);
                std::vector<Vertex> vertices;
                std::vector<uint32_t> indices;

		static std::vector<std::shared_ptr<Mesh>> ReadMeshFromFile(const std::string& path);
        };
}
