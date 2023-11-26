#include <utility>
#include <Cardia/Application.hpp>

#include "Cardia/Renderer/Material.hpp"
#include "Cardia/Renderer/Renderer.hpp"


namespace Cardia
{
	Material Material::Builder::Build()
	{
		return { m_Device, m_MaterialData };
	}

	Material::Material(const Device &device, MaterialData materialData)
		: m_Device(device), m_MaterialData(std::move(materialData))
	{
		CreateDescriptorSet();
	}

	void Material::Bind(const Pipeline& pipeline, VkCommandBuffer commandBuffer) const
	{
		vkCmdBindDescriptorSets(
			commandBuffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			pipeline.GetLayout(),
			1, 1,
			&m_DescriptorSet->GetDescriptor(),
			0, nullptr);
	}

	void Material::CreateDescriptorSet()
	{
		auto& renderer = Application::Get().GetRenderer();

		m_MaterialBuffer = std::make_unique<Buffer>(
			m_Device,
			sizeof(Vector4f),
			1,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

		m_MaterialBuffer->UploadData(sizeof(Vector4f), &m_MaterialData.AlbedoColor);

		DescriptorSet::Writer descriptorWriter(renderer.GetDescriptorAllocator(), renderer.GetMaterialDescriptorSetLayout());

		auto& assetsManager = Application::Get().GetAssetsManager();
		const auto albedoMap = assetsManager.Load<Texture>(m_MaterialData.AlbedoMap);
		const auto normalMap = assetsManager.Load<Texture>(m_MaterialData.NormalMap);
		const auto metallicRoughnessMap = assetsManager.Load<Texture>(m_MaterialData.MetallicRoughnessMap);
		const auto AOMap = assetsManager.Load<Texture>(m_MaterialData.AOMap);

		const auto bufferInfo = m_MaterialBuffer->DescriptorInfo();

		m_DescriptorSet = descriptorWriter
			.WriteImage(0, albedoMap->GetImageInfo())
			.WriteImage(1, normalMap->GetImageInfo())
			.WriteImage(2, metallicRoughnessMap->GetImageInfo())
			.WriteImage(3, AOMap->GetImageInfo())
			.WriteBuffer(4, &bufferInfo)
			.Build();
	}
}
