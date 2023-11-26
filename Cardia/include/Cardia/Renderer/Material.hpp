#pragma once

#include <Cardia/Math/Vector4.hpp>
#include <any>
#include "Pipeline.hpp"
#include "Texture.hpp"
#include "Buffer.hpp"


namespace Cardia
{
	struct MaterialData
	{
		AssetHandle AlbedoMap = AssetHandle::Invalid();
		AssetHandle NormalMap = AssetHandle::Invalid();

		AssetHandle MetallicRoughnessMap = AssetHandle::Invalid();
		AssetHandle AOMap = AssetHandle::Invalid();
		AssetHandle EmissiveMap = AssetHandle::Invalid();

		Vector4f AlbedoColor = Vector4f(1.0f);
		float Metallic = 0.0f;
		float Roughness = 0.0f;
		Vector3f EmissiveFactor = Vector3f(0.0f);


		constexpr static auto properties = std::make_tuple(
			property(&MaterialData::AlbedoMap, "AlbedoMap"),
			property(&MaterialData::NormalMap, "NormalMap"),
			property(&MaterialData::MetallicRoughnessMap, "MetallicRoughnessMap"),
			property(&MaterialData::AOMap, "AOMap"),
			property(&MaterialData::EmissiveMap, "EmissiveMap"),
			property(&MaterialData::AlbedoColor, "AlbedoColor"),
			property(&MaterialData::Metallic, "Metallic"),
			property(&MaterialData::Roughness, "Roughness"),
			property(&MaterialData::EmissiveFactor, "EmissiveFactor")
		);
	};


	class Material
	{
	public:
		class Builder {
		public:
			explicit Builder(const Device& device) : m_Device(device) {}
			Builder& SetAlbedoMap(const AssetHandle& handle) { m_MaterialData.AlbedoMap = handle; return *this; }
			Builder& SetNormalMap(const AssetHandle& handle) { m_MaterialData.NormalMap = handle; return *this; }
			Builder& SetMetallicRoughnessMap(const AssetHandle& handle) { m_MaterialData.MetallicRoughnessMap = handle; return *this; }
			Builder& SetAOMap(const AssetHandle& handle) { m_MaterialData.AOMap = handle; return *this; }
			Builder& SetEmissiveMap(const AssetHandle& handle) { m_MaterialData.EmissiveMap = handle; return *this; }

			Builder& SetAlbedoColor(const Vector4f& color) { m_MaterialData.AlbedoColor = color; return *this; }
			Builder& SetMetallicFactor(float metallic) { m_MaterialData.Metallic = metallic; return *this; }
			Builder& SetRoughnessFactor(float roughness) { m_MaterialData.Roughness = roughness; return *this; }
			Builder& SetEmissiveFactor(Vector3f emissive) { m_MaterialData.EmissiveFactor = emissive; return *this; }

			Material Build();

		private:
			const Device& m_Device;
			MaterialData m_MaterialData;
		};

	public:
		Material(const Device& device, MaterialData materialData);
		void Bind(const Pipeline& pipeline, VkCommandBuffer commandBuffer) const;

	private:
		void CreateDescriptorSet();
		const Device& m_Device;
		MaterialData m_MaterialData;
		std::unique_ptr<DescriptorSet> m_DescriptorSet;
		std::unique_ptr<Buffer> m_MaterialBuffer;
	};
}
