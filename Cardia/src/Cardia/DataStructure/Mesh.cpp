#include "cdpch.hpp"
#include "Cardia/DataStructure/Mesh.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <Cardia/Core/Log.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>


namespace Cardia
{
	Mesh Mesh::ReadMeshFromFile(const std::string &path)
	{
		Mesh mesh;
		Assimp::Importer importer;
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn, err;

		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str())) {
			throw std::runtime_error(warn + err);
		}

		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		for (const auto& shape : shapes) {
			for (const auto& index : shape.mesh.indices) {
				auto& vertex = vertices.emplace_back();
				indices.push_back(indices.size());

				vertex.position = {
					attrib.vertices[3 * index.vertex_index + 0],
					attrib.vertices[3 * index.vertex_index + 1],
					attrib.vertices[3 * index.vertex_index + 2]
				};

				vertex.normal = {
					attrib.normals[3 * index.normal_index + 0],
					attrib.normals[3 * index.normal_index + 1],
					attrib.normals[3 * index.normal_index + 2]
				};
				vertex.textureCoord = {
					attrib.texcoords[2 * index.texcoord_index + 0] / 2,
					attrib.texcoords[2 * index.texcoord_index + 1] / 2
				};
				vertex.color = {1.0f, 1.0f, 1.0f, 1.0f};
				vertex.tilingFactor = 1;
			}
		}
		auto& subMesh = mesh.GetSubMeshes().emplace_back();
		subMesh.GetVertices() = vertices;
		subMesh.GetIndices() = indices;

		return mesh;
	}
}
