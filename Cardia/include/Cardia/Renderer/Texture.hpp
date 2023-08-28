#pragma once

#include <memory>
#include <string>

#include "Device.hpp"
#include "Descriptors.hpp"
#include "Cardia/Asset/Asset.hpp"


namespace Cardia
{

	class Renderer;
	class Texture2D final : public Asset
	{
	public:
		void Reload(const std::filesystem::path& path) override;

		Texture2D(const Texture2D& other) = delete;
		Texture2D& operator=(const Texture2D& other) = delete;
		Texture2D(Texture2D&& other) noexcept;
		Texture2D& operator=(Texture2D&& other) noexcept;

		Texture2D(Device& device, Renderer& renderer, const std::filesystem::path& path);
		Texture2D(
			Device& device,
			Renderer& renderer,
			const VkExtent2D& size,
			VkFormat format = VK_FORMAT_R8G8B8A8_SRGB,
			VkImageUsageFlags usageFlags = VK_IMAGE_USAGE_SAMPLED_BIT,
			VkImageAspectFlags aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT);

		Texture2D(Device& device, Renderer& renderer, const VkExtent2D& size, const void* data);

		~Texture2D() override;

		uint32_t GetHeight() const { return m_Size.height; }
		uint32_t GetWidth() const { return m_Size.width; }

		VkSampler GetSampler() const { return m_TextureSampler; }
		VkImageView GetView() const { return m_TextureImageView; }
		VkDescriptorImageInfo GetImageInfo() const;

	private:
		void Init(const std::filesystem::path& path);
		void Release();

		void CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageUsageFlags usageFlags, VkImageAspectFlags aspectFlags);
		void TransitionImageLayout(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, VkImageLayout oldLayout, VkImageLayout newLayout) const;
		void CreateImageView(VkFormat format, VkImageAspectFlags aspectFlags);
		void CreateTextureSampler();

		VkExtent2D m_Size {};
		
		Device& m_Device;
		Renderer& m_Renderer;
		VkImage m_TextureImage {};
		VkDeviceMemory m_TextureImageMemory {};
		VkImageView m_TextureImageView {};
		VkSampler m_TextureSampler {};
	};
}
