#include <utility>

#include "Cardia/Renderer/Material.hpp"
#include "Cardia/Renderer/Renderer.hpp"


namespace Cardia
{
	MaterialInstance::MaterialInstance(Renderer &renderer, Material  material)
		: m_Renderer(renderer), m_Material(std::move(material))
	{
		CreateDescriptorSet();
	}

	void MaterialInstance::CreateDescriptorSet()
	{
		DescriptorSet::Writer descriptorWriter(m_Renderer.GetDescriptorAllocator(), m_Renderer.GetMaterialDescriptorSetLayout());

		m_DescriptorSet = descriptorWriter
			.WriteImage(0, m_Material.AlbedoMap->GetImageInfo())
			.WriteImage(1, m_Material.NormalMap->GetImageInfo())
			.WriteImage(2, m_Material.MetallicRoughnessMap->GetImageInfo())
			.WriteImage(3, m_Material.AOMap->GetImageInfo())
			.Build();
	}

	void MaterialInstance::Bind(VkCommandBuffer commandBuffer) const
	{
		vkCmdBindDescriptorSets(
			commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			m_Renderer.GetMainPipeline().GetLayout(),
			1, 1,
			&m_DescriptorSet->GetDescriptor(),
			0, nullptr);
	}
}
