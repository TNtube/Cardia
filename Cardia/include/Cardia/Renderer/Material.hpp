#pragma once

#include <Cardia/Math/Vector4.hpp>
#include <any>
#include "Pipeline.hpp"
#include "Texture.hpp"


namespace Cardia
{
	class Material : public Asset
	{
	public:
		std::shared_ptr<Texture> AlbedoMap = nullptr;
		std::shared_ptr<Texture> MetallicRoughnessMap = nullptr;

		std::shared_ptr<Texture> NormalMap = nullptr;
		std::shared_ptr<Texture> AOMap = nullptr;
		std::shared_ptr<Texture> EmissiveMap = nullptr;

		Vector4f AlbedoColor = Vector4f(1.0f);
		float Metallic = 0.0f;
		float Roughness = 0.0f;
		Vector3f EmissiveFactor = Vector3f(0.0f);

		void Reload() override {};
	};

	class MaterialInstance
	{
	public:
		explicit MaterialInstance(Renderer& renderer, Material material);

		void Bind(VkCommandBuffer commandBuffer) const;
	private:
		void CreateDescriptorSet();
		Renderer& m_Renderer;

		Material m_Material;

		std::shared_ptr<DescriptorSetLayout> m_DescriptorSetLayout;
		std::unique_ptr<DescriptorSet> m_DescriptorSet;
	};
}
