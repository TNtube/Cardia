#pragma once

#include <Cardia/Math/Vector4.hpp>
#include <any>
#include "Pipeline.hpp"
#include "Texture.hpp"


namespace Cardia
{
	class Material
	{
	public:
		std::shared_ptr<Texture2D> AlbedoMap = nullptr;
		std::shared_ptr<Texture2D> MetallicRoughnessMap = nullptr;

		std::shared_ptr<Texture2D> NormalMap = nullptr;
		std::shared_ptr<Texture2D> AOMap = nullptr;
		std::shared_ptr<Texture2D> EmissiveMap = nullptr;

		Vector4f AlbedoColor = Vector4f(1.0f);
		float Metallic = 0.0f;
		float Roughness = 0.0f;
		Vector3f EmissiveFactor = Vector3f(0.0f);
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
