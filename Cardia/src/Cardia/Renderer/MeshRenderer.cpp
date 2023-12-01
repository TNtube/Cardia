#include "cdpch.hpp"
#include <Cardia/Application.hpp>
#include "Cardia/Renderer/Texture.hpp"

#include "Cardia/Renderer/MeshRenderer.hpp"

namespace Cardia
{

	MeshRenderer::MeshRenderer(const Device &device, UUID id, Model model)
		: m_Device(device), m_Model(std::move(model))
	{
		Init();
	}

	void MeshRenderer::Draw(const Pipeline& pipeline, VkCommandBuffer commandBuffer) const
	{
		for (size_t i = 0; i < m_SubMeshRenderers.size(); i++)
		{
			const auto materialIndex = m_Model.GetMeshes()[i].GetMaterialIndex();
			if (m_Materials.size() > materialIndex)
			{
				auto& materialInstance = m_Materials[materialIndex];
				materialInstance->Bind(pipeline, commandBuffer);
			}
			m_SubMeshRenderers[i].Bind(commandBuffer);
			m_SubMeshRenderers[i].Draw(commandBuffer);
		}
	}

	void MeshRenderer::Init()
	{
		auto& assetsManager = Application::Get().GetAssetsManager();
		for (const auto& material : m_Model.GetMaterialHandles())
		{
			auto mat = assetsManager.Load<Material>(material);
			m_Materials.emplace_back(mat);
		}

		for (auto& mesh : m_Model.GetMeshes())
		{
			m_SubMeshRenderers.emplace_back(m_Device, mesh);
		}
	}
}