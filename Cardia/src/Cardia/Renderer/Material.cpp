#include <utility>
#include <Cardia/Application.hpp>

#include "Cardia/Renderer/Material.hpp"
#include "Cardia/Renderer/Renderer.hpp"


namespace Cardia
{
	Material Material::Builder::Build()
	{
		CdCoreAssert(m_Handle.IsValid(), "Invalid handle for material");
		return { m_Device, m_MaterialData, m_Handle };
	}

	Material::Material(const Device &device, MaterialData materialData, AssetHandle assetHandle)
		: Asset(std::move(assetHandle)), m_Device(device), m_MaterialData(std::move(materialData))
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
		auto albedoMap = assetsManager.Load<Texture>(m_MaterialData.AlbedoMap);
		auto normalMap = assetsManager.Load<Texture>(m_MaterialData.NormalMap);
		auto metallicRoughnessMap = assetsManager.Load<Texture>(m_MaterialData.MetallicRoughnessMap);
		auto AOMap = assetsManager.Load<Texture>(m_MaterialData.AOMap);

		auto bufferInfo = m_MaterialBuffer->DescriptorInfo();

		m_DescriptorSet = descriptorWriter
			.WriteImage(0, albedoMap->GetImageInfo())
			.WriteImage(1, normalMap->GetImageInfo())
			.WriteImage(2, metallicRoughnessMap->GetImageInfo())
			.WriteImage(3, AOMap->GetImageInfo())
			.WriteBuffer(4, &bufferInfo)
			.Build();
	}

	void Material::Reload()
	{
		CreateDescriptorSet();
	}

	Json::Value MaterialData::Serialize() const
	{
		Json::Value output;

		output["AlbedoMap"] = AlbedoMap.ID.ToString();
		output["NormalMap"] = NormalMap.ID.ToString();
		output["MetallicRoughnessMap"] = MetallicRoughnessMap.ID.ToString();
		output["AOMap"] = AOMap.ID.ToString();
		output["EmissiveMap"] = EmissiveMap.ID.ToString();
		output["AlbedoColor"] = AlbedoColor.Serialize();
		output["MetallicFactor"] = Metallic;
		output["RoughnessFactor"] = Roughness;
		output["EmissiveFactor"] = EmissiveFactor.Serialize();

		return output;
	}

	std::optional<MaterialData> MaterialData::Deserialize(const Json::Value& root)
	{
		MaterialData data;

		if (root.isMember("AlbedoMap")) {
			data.AlbedoMap = AssetHandle(UUID::FromString(root["AlbedoMap"].asString()));
		}
		if (root.isMember("NormalMap")) {
			data.NormalMap = AssetHandle(UUID::FromString(root["NormalMap"].asString()));
		}
		if (root.isMember("MetallicRoughnessMap")) {
			data.MetallicRoughnessMap = AssetHandle(UUID::FromString(root["MetallicRoughnessMap"].asString()));
		}
		if (root.isMember("AOMap")) {
			data.AOMap = AssetHandle(UUID::FromString(root["AOMap"].asString()));
		}
		if (root.isMember("EmissiveMap")) {
			data.EmissiveMap = AssetHandle(UUID::FromString(root["EmissiveMap"].asString()));
		}
		if (root.isMember("AlbedoColor")) {
			data.AlbedoColor = *Vector4f::Deserialize(root["AlbedoColor"]);
		}
		if (root.isMember("MetallicFactor")) {
			data.Metallic = root["MetallicFactor"].asFloat();
		}
		if (root.isMember("RoughnessFactor")) {
			data.Roughness = root["RoughnessFactor"].asFloat();
		}
		if (root.isMember("EmissiveFactor")) {
			data.EmissiveFactor = *Vector3f::Deserialize(root["EmissiveFactor"]);
		}

		return data;
	}
}
