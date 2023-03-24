#include <Cardia/Renderer/Texture.hpp>
#include <Cardia/Project/AssetsManager.hpp>
#include "cdpch.hpp"

#include "Cardia/Renderer/MeshRenderer.hpp"

namespace Cardia
{

	void MeshRenderer::SubmitMesh(std::shared_ptr<Mesh> mesh)
	{
		m_Mesh = std::move(mesh);
		m_SubMeshRenderers.erase(m_SubMeshRenderers.begin(), m_SubMeshRenderers.end());
		auto& subMeshes = m_Mesh->GetSubMeshes();

		for (auto& subMesh : subMeshes)
		{
			auto& subMeshRender = m_SubMeshRenderers.emplace_back();
			subMeshRender.SubmitSubMesh(subMesh);
		}
	}

	void MeshRenderer::Draw()
	{
		for (size_t i = 0; i < m_SubMeshRenderers.size(); i++)
		{
			auto& materials = m_Mesh->GetMaterials();
			const auto materialIndex = m_Mesh->GetSubMeshes()[i].GetMaterialIndex();
			if (materials.size() > materialIndex)
				materials[materialIndex]->bind(0);
			else
			{
				auto whiteTexture = AssetsManager::Load<Texture2D>("resources/textures/white.jpg", AssetsManager::LoadType::Editor);
				whiteTexture->bind(0);
			}
			m_SubMeshRenderers[i].Draw();
		}
	}
}