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
		for (const auto& m_SubMeshRenderer : m_SubMeshRenderers)
		{
			m_SubMeshRenderer.Bind(commandBuffer);
			m_SubMeshRenderer.Draw(commandBuffer);
		}
	}
}