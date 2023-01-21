#include "cdpch.hpp"

#include "Cardia/Renderer/MeshRenderer.hpp"

namespace Cardia
{

	void MeshRenderer::SubmitMesh(Mesh &mesh)
	{
		m_SubMeshRenderers.erase(m_SubMeshRenderers.begin(), m_SubMeshRenderers.end());
		auto& subMeshes = mesh.GetSubMeshes();

		for (auto& subMesh : subMeshes)
		{
			auto& subMeshRender = m_SubMeshRenderers.emplace_back();
			subMeshRender.SubmitSubMesh(subMesh);
		}
	}

	void MeshRenderer::Draw()
	{
		for (auto& subMeshRender: m_SubMeshRenderers)
		{
			subMeshRender.Draw();
		}
	}
}