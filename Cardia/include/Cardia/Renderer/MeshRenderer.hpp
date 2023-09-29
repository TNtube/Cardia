#pragma once

#include <vector>
#include <memory>
#include "Cardia/DataStructure/Model.hpp"
#include "SubMeshRenderer"


namespace Cardia
{
	class MeshRenderer : public Asset
	{
	public:
		explicit MeshRenderer(const Device& device, AssetHandle handle, Model model);
		const Model& GetModel() const { return m_Model; }
		void Draw(const Pipeline& pipeline, VkCommandBuffer commandBuffer) const;

		void Reload() override;
	private:
		const Device& m_Device;

		void Init();
		Model m_Model {};
		std::vector<std::shared_ptr<Material>> m_Materials {};
		std::vector<SubMeshRenderer> m_SubMeshRenderers {};
	};
}