#include "cdpch.hpp"
#include "Cardia/Renderer/Mesh.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <Cardia/Core/Log.hpp>


namespace Cardia
{
	Mesh::Mesh(std::vector<Vertex> verts, std::vector<uint32_t> inds)
		: vertices(std::move(verts)), indices(std::move(inds))
		{}

	std::vector<std::shared_ptr<Mesh>> Mesh::ReadMeshFromFile(const std::string &path)
	{
		std::vector<std::shared_ptr<Mesh>> meshes;
		Assimp::Importer importer;
		const aiScene *scene = importer.ReadFile(path,
							 aiProcess_FlipUVs |
							 aiProcess_JoinIdenticalVertices |
							 aiProcess_OptimizeMeshes
							 );

		if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			Log::coreError("Failed to load model : {0}", importer.GetErrorString());
			return {};
		}

		Log::coreWarn("Num of meshes loaded : {0}", scene->mNumMeshes);
		for (int ind = 0; ind < std::min(5u, scene->mNumMeshes); ind++) {
			std::vector<Vertex> vertices;
			std::vector<uint32_t> indices;
			aiMesh* ai_mesh = scene->mMeshes[ind];
			// walk through each of the mesh's vertices
			vertices.reserve(ai_mesh->mNumVertices);
			for(unsigned i = 0; i < ai_mesh->mNumVertices; i++) {
				Vertex vertex{};
				vertex.color = {1.0f, 0.5f, 0.2f, 1.0f};
				glm::vec3 vector;
				vector.x = ai_mesh->mVertices[i].x;
				vector.y = ai_mesh->mVertices[i].y;
				vector.z = ai_mesh->mVertices[i].z;
				if (ai_mesh->HasNormals())
				{
					vector.x = ai_mesh->mNormals[i].x;
					vector.y = ai_mesh->mNormals[i].y;
					vector.z = ai_mesh->mNormals[i].z;
					vertex.normal = vector;
				}

				if(ai_mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
				{
					glm::vec2 vec;
					// a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't
					// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
					vec.x = ai_mesh->mTextureCoords[0][i].x;
					vec.y = ai_mesh->mTextureCoords[0][i].y;
					vertex.textureCoord = vec;
				}
				else
					vertex.textureCoord = glm::vec2(0.0f, 0.0f);
				vertices.emplace_back(vertex);
			}

			for (unsigned int faceIndex = 0; faceIndex < ai_mesh->mNumFaces; faceIndex++)
			{
				aiFace face = ai_mesh->mFaces[faceIndex];
				// retrieve all indices of the face and store them in the indices vector
				indices.reserve(indices.size() + face.mNumIndices);
				indices.insert(indices.end(), face.mIndices, face.mIndices + face.mNumIndices);
			}
			meshes.emplace_back(std::make_shared<Mesh>(vertices, indices));
		}

		return meshes;
	}
}
