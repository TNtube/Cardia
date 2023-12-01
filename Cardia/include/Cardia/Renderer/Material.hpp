#pragma once

#include <Cardia/Math/Vector4.hpp>
#include <any>
#include "Pipeline.hpp"
#include "Texture.hpp"
#include "Buffer.hpp"
#include "Cardia/Core/UUID.hpp"


namespace Cardia
{
	struct MaterialData
	{
		UUID AlbedoMap = UUID::Default();
		UUID NormalMap = UUID::Default();

		UUID MetallicRoughnessMap = UUID::Default();
		UUID AOMap = UUID::Default();
		UUID EmissiveMap = UUID::Default();

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
			Builder& SetAlbedoMap(const UUID& id) { m_MaterialData.AlbedoMap = id; return *this; }
			Builder& SetNormalMap(const UUID& id) { m_MaterialData.NormalMap = id; return *this; }
			Builder& SetMetallicRoughnessMap(const UUID& id) { m_MaterialData.MetallicRoughnessMap = id; return *this; }
			Builder& SetAOMap(const UUID& id) { m_MaterialData.AOMap = id; return *this; }
			Builder& SetEmissiveMap(const UUID& id) { m_MaterialData.EmissiveMap = id; return *this; }

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
