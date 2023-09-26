#include "cdpch.hpp"
#include "Cardia/DataStructure/Model.hpp"
#include "Cardia/Application.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <Cardia/Core/Log.hpp>
#include <Cardia/Asset/AssetsManager.hpp>
#include <Cardia/DataStructure/Vertex.hpp>

#include "Cardia/Math/Vector4.hpp"


namespace Cardia
{
	namespace {
		std::filesystem::path GetPathRelativeToMesh(const std::filesystem::path& meshPath, const std::filesystem::path& path)
		{
			auto parentPath = meshPath.parent_path();
			return parentPath / path;
		}

		std::vector<AssetHandle> ProcessMaterialsFromScene(const std::filesystem::path& modelPath, const aiScene* scene) {
			auto& assetsManager = Application::Get().GetAssetsManager();

			std::vector<AssetHandle> result;

			for (uint32_t i = 0; i < scene->mNumMaterials; i++)
			{
				aiMaterial* pMaterial = scene->mMaterials[i];
				MaterialData mat;

				aiString texturePath;
				aiColor4D textureColor;
				ai_real textureFloat;

				if (AI_SUCCESS == aiGetMaterialColor(pMaterial, AI_MATKEY_COLOR_DIFFUSE, &textureColor))
					mat.AlbedoColor = Vector4f(textureColor.r, textureColor.g, textureColor.b, textureColor.a);
				if (AI_SUCCESS == pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath))
					mat.AlbedoMap = assetsManager.GetHandleFromAbsolute(GetPathRelativeToMesh(modelPath, texturePath.C_Str()));
				else
					mat.AlbedoMap = assetsManager.GetHandleFromAbsolute("resources/textures/white.jpg");

				if (AI_SUCCESS == aiGetMaterialFloat(pMaterial, AI_MATKEY_METALLIC_FACTOR, &textureFloat))
					mat.Metallic = textureFloat;
				if (AI_SUCCESS == aiGetMaterialFloat(pMaterial, AI_MATKEY_ROUGHNESS_FACTOR, &textureFloat))
					mat.Roughness = textureFloat;
				if (AI_SUCCESS == pMaterial->GetTexture(aiTextureType_METALNESS, 0, &texturePath))
					mat.MetallicRoughnessMap = assetsManager.GetHandleFromAbsolute(GetPathRelativeToMesh(modelPath, texturePath.C_Str()));
				else
					mat.MetallicRoughnessMap = assetsManager.GetHandleFromAbsolute("resources/textures/white.jpg");

				if (AI_SUCCESS == pMaterial->GetTexture(aiTextureType_NORMALS, 0, &texturePath))
					mat.NormalMap = assetsManager.GetHandleFromAbsolute(GetPathRelativeToMesh(modelPath, texturePath.C_Str()));
				else
					mat.NormalMap = assetsManager.GetHandleFromAbsolute("resources/textures/normal.jpg");

				if (AI_SUCCESS == pMaterial->GetTexture(aiTextureType_AMBIENT_OCCLUSION, 0, &texturePath))
					mat.AOMap = assetsManager.GetHandleFromAbsolute(GetPathRelativeToMesh(modelPath, texturePath.C_Str()));
				else
					mat.AOMap = assetsManager.GetHandleFromAbsolute("resources/textures/white.jpg");

				if (AI_SUCCESS == aiGetMaterialColor(pMaterial, AI_MATKEY_EMISSIVE_INTENSITY, &textureColor))
					mat.EmissiveFactor = Vector3f(textureColor.r, textureColor.g, textureColor.b);
				if (AI_SUCCESS == pMaterial->GetTexture(aiTextureType_EMISSIVE, 0, &texturePath))
					mat.EmissiveMap = assetsManager.GetHandleFromAbsolute(GetPathRelativeToMesh(modelPath, texturePath.C_Str()));
				else
					mat.EmissiveMap = assetsManager.GetHandleFromAbsolute("resources/textures/white.jpg");

				std::filesystem::path matFilename = pMaterial->GetName().C_Str();
				auto absoluteMatPath = GetPathRelativeToMesh(modelPath, matFilename.string() + ".mat");

				Serializer<MaterialData> matSerializer(mat);
				matSerializer.Serialize(absoluteMatPath);

				AssetHandle matHandle = assetsManager.AddEntry(absoluteMatPath);
				Serializer<AssetHandle> handleSerializer(matHandle);
				handleSerializer.Serialize(absoluteMatPath.string() +  ".imp");

				result.push_back(matHandle);
			}

			return result;
		}

		std::vector<AssetHandle> ProcessImportedMaterials(const Json::Value& root) {
			std::vector<AssetHandle> result;

			// process json
			for (const Json::Value& jsonMat : root["Materials"])
			{
				result.emplace_back(UUID::FromString(jsonMat.asString()));
			}

			return result;
		}
	}
	Model Model::FromFile(const std::filesystem::path& absolutePath, bool includeMaterials)
	{
		Model model;
		Assimp::Importer importer;
		const aiScene *scene = importer.ReadFile(absolutePath.string(), aiProcessPreset_TargetRealtime_Fast);

		if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			Log::CoreError("Failed to load model : {0}", importer.GetErrorString());
			return {};
		}

		// Materials
		if (includeMaterials) {
			auto impFilePath = absolutePath.string() + ".imp";
			std::ifstream file(impFilePath);
			Json::Value root;
			file >> root;
			file.close();


			if (root.isMember("Materials")) {
				model.m_MaterialHandles = ProcessImportedMaterials(impFilePath);
			} else {
				model.m_MaterialHandles = ProcessMaterialsFromScene(absolutePath, scene);
			}
		}

		for (uint32_t ind = 0; ind < scene->mNumMeshes; ind++) {
			auto& mesh = model.m_Meshes.emplace_back();
			std::vector<Vertex>& vertices = mesh.GetVertices();
			std::vector<uint32_t>& indices = mesh.GetIndices();

			aiMesh* ai_mesh = scene->mMeshes[ind];

			mesh.m_MaterialIndex = ai_mesh->mMaterialIndex;
			// walk through each of the model's vertices
			vertices.reserve(ai_mesh->mNumVertices);
			for(unsigned i = 0; i < ai_mesh->mNumVertices; i++) {
				Vertex vertex{};

				vertex.Position.x = ai_mesh->mVertices[i].x;
				vertex.Position.y = ai_mesh->mVertices[i].y;
				vertex.Position.z = ai_mesh->mVertices[i].z;
				if (ai_mesh->HasNormals())
				{
					vertex.Normal.x = ai_mesh->mNormals[i].x;
					vertex.Normal.y = ai_mesh->mNormals[i].y;
					vertex.Normal.z = ai_mesh->mNormals[i].z;
				}
				
				if (ai_mesh->mTextureCoords[0]) // does the model contain texture coordinates?
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

		return model;
	}
}
