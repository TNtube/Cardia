#include "cdpch.hpp"
#include "Cardia/DataStructure/Mesh.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <Cardia/Core/Log.hpp>

#include "Cardia/Math/Vector4.hpp"


namespace Cardia
{
	std::shared_ptr<Mesh> Mesh::ReadMeshFromFile(const std::string &path)
	{
		std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
		Assimp::Importer importer;
		const aiScene *scene = importer.ReadFile(path, aiProcessPreset_TargetRealtime_Fast);

		if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			Log::CoreError("Failed to load model : {0}", importer.GetErrorString());
			return {};
		}

		Log::CoreWarn("Num of meshes loaded : {0}", scene->mNumMeshes);
		for (uint32_t ind = 0; ind < scene->mNumMeshes; ind++) {
			auto& subMesh = mesh->GetSubMeshes().emplace_back();
			std::vector<SubMesh::Vertex>& vertices = subMesh.GetVertices();
			std::vector<uint32_t>& indices = subMesh.GetIndices();

			aiMesh* ai_mesh = scene->mMeshes[ind];

			subMesh.GetMaterialIndex() = ai_mesh->mMaterialIndex - 1;
			// walk through each of the mesh's vertices
			vertices.reserve(ai_mesh->mNumVertices);
			for(unsigned i = 0; i < ai_mesh->mNumVertices; i++) {
				SubMesh::Vertex vertex{};
				vertex.Color = Vector4f(1.0f);

				vertex.Position.x = ai_mesh->mVertices[i].x;
				vertex.Position.y = ai_mesh->mVertices[i].y;
				vertex.Position.z = ai_mesh->mVertices[i].z;
				if (ai_mesh->HasNormals())
				{
					vertex.Normal.x = ai_mesh->mNormals[i].x;
					vertex.Normal.y = ai_mesh->mNormals[i].y;
					vertex.Normal.z = ai_mesh->mNormals[i].z;
				}
				
				if (ai_mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
				{
					// a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't
					// use models where a vertex can have multiple texture coordinates, so we always take the first set (0).
					vertex.TextureCoord.x = ai_mesh->mTextureCoords[0][i].x;
					vertex.TextureCoord.y = 1.0f - ai_mesh->mTextureCoords[0][i].y;
				}
				else
					vertex.TextureCoord = Vector2f(0.0f, 0.0f);
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
