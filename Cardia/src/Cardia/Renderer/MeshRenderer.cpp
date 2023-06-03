#include "cdpch.hpp"
#include <Cardia/Renderer/Texture.hpp>
#include <Cardia/Project/AssetsManager.hpp>
#include "cdpch.hpp"

#include "Cardia/Renderer/MeshRenderer.hpp"

namespace Cardia
{

	void MeshRenderer::SubmitMesh(Device& device, const std::shared_ptr<Mesh>& mesh)
	{
		m_Mesh = mesh;
		auto& subMeshes = mesh->GetSubMeshes();

		for (auto& subMesh : subMeshes)
		{
			auto& subMeshRender = m_SubMeshRenderers.emplace_back(device, subMesh);
			subMeshRender.SubmitSubMesh(subMesh);
		}
	}

	void MeshRenderer::Draw(VkCommandBuffer commandBuffer) const
	{
		auto& materials = m_Mesh->GetMaterials();
		for (size_t i = 0; i < m_SubMeshRenderers.size(); i++)
		{
			const auto materialIndex = m_Mesh->GetSubMeshes()[i].GetMaterialIndex();
			if (materials.size() > materialIndex)
			{
				Texture2D& texture = *materials[materialIndex];
				texture.Bind(commandBuffer);
			}
			m_SubMeshRenderers[i].Bind(commandBuffer);
			m_SubMeshRenderers[i].Draw(commandBuffer);
		}
	}
}