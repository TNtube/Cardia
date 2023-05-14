#include "cdpch.hpp"
#include "Cardia/DataStructure/Mesh.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <Cardia/Core/Log.hpp>


namespace Cardia
{
	Mesh Mesh::ReadMeshFromFile(const std::string &path)
	{
		Mesh mesh;
		Assimp::Importer importer;
		const aiScene *scene = importer.ReadFile(path, aiProcessPreset_TargetRealtime_Fast);

		if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			Log::coreError("Failed to load model : {0}", importer.GetErrorString());
			return {};
		}

		Log::coreWarn("Num of meshes loaded : {0}", scene->mNumMeshes);
		for (int ind = 0; ind < scene->mNumMeshes; ind++) {
			auto& subMesh = mesh.GetSubMeshes().emplace_back();
			std::vector<SubMesh::Vertex>& vertices = subMesh.GetVertices();
			std::vector<uint32_t>& indices = subMesh.GetIndices();

			aiMesh* ai_mesh = scene->mMeshes[ind];

			subMesh.GetMaterialIndex() = ai_mesh->mMaterialIndex - 1;
			// walk through each of the mesh's vertices
			vertices.reserve(ai_mesh->mNumVertices);
			for(unsigned i = 0; i < ai_mesh->mNumVertices; i++) {
				SubMesh::Vertex vertex{};
				// vertex.color = glm::vec4(1);

				vertex.position.x = ai_mesh->mVertices[i].x;
				vertex.position.y = ai_mesh->mVertices[i].y;
				vertex.position.z = ai_mesh->mVertices[i].z;
				// if (ai_mesh->HasNormals())
				// {
				// 	vertex.normal.x = ai_mesh->mNormals[i].x;
				// 	vertex.normal.y = ai_mesh->mNormals[i].y;
				// 	vertex.normal.z = ai_mesh->mNormals[i].z;
				// }
				//
				// if (ai_mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
				// {
				// 	// a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't
				// 	// use models where a vertex can have multiple texture coordinates, so we always take the first set (0).
				// 	vertex.textureCoord.x = ai_mesh->mTextureCoords[0][i].x;
				// 	vertex.textureCoord.y = ai_mesh->mTextureCoords[0][i].y;
				// }
				// else
				// 	vertex.textureCoord = glm::vec2(0.0f, 0.0f);
				vertices.emplace_back(vertex);
			}

			for (unsigned int faceIndex = 0; faceIndex < ai_mesh->mNumFaces; faceIndex++)
			{
				aiFace face = ai_mesh->mFaces[faceIndex];
				// retrieve all indices of the face and store them in the indices vector
				indices.reserve(indices.size() + face.mNumIndices);
				indices.insert(indices.end(), face.mIndices, face.mIndices + face.mNumIndices);
			}
		}

		return mesh;
	}
}
