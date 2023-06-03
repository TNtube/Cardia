#pragma once

#include <memory>
#include <string>
#include <filesystem>
#include <vulkan/vulkan.h>

#include "Device.hpp"
#include "Descriptors.hpp"


namespace Cardia
{

	class Renderer;
	class Texture2D
	{
	public:
		Texture2D(Device& device, Renderer& renderer, const std::filesystem::path& path);
		virtual ~Texture2D();

		virtual uint32_t GetHeight() const { return m_Height; }
		virtual uint32_t GetWidth() const { return m_Width; }

		VkSampler GetSampler() const { return m_TextureSampler; }
		DescriptorSet& GetDescriptorSet() const { return *m_TextureDescriptorSet; }
		
		void Bind(VkCommandBuffer commandBuffer) const;

		static std::unique_ptr<Texture2D> create(const std::string& path);
		static std::unique_ptr<Texture2D> create(int width, int height, void* data);

	private:
		void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout) const;
		void CreateImageView(VkFormat format, VkImageAspectFlags aspectFlags);
		void CreateTextureSampler();

		uint32_t m_Width {};
		uint32_t m_Height {};
		
		Device& m_Device;
		Renderer& m_Renderer;
		VkImage m_TextureImage {};
		VkDeviceMemory m_TextureImageMemory {};
		VkImageView m_TextureImageView {};
		VkSampler m_TextureSampler {};
		std::unique_ptr<DescriptorSet> m_TextureDescriptorSet {};
	};
}
