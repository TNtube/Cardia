#pragma once

#include "Device.hpp"
#include "Pipeline.hpp"
#include "Texture.hpp"
#include "Descriptors.hpp"
#include "Buffer.hpp"

namespace Cardia
{
	class Skybox
	{
	public:
		Skybox(const Renderer& device, const AssetHandle& assetHandle, const std::filesystem::path& path);

		Pipeline& GetPipeline() const { return *m_SkyboxPipeline; }

		void Draw(VkCommandBuffer commandBuffer) const;
	private:
		const Renderer& m_Renderer;

		std::unique_ptr<Pipeline> m_SkyboxPipeline {};
		std::unique_ptr<Texture> m_SkyboxTexture {};
		std::unique_ptr<DescriptorSet> m_SkyboxTextureDescriptorSet {};

		uint32_t m_VertexCount;
		uint32_t m_IndexCount;
		std::unique_ptr<Buffer> m_VertexBuffer;
		std::unique_ptr<Buffer> m_IndexBuffer;
	};
}