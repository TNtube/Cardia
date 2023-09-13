#pragma once

#include <memory>
#include <string>

#include "Device.hpp"
#include "Descriptors.hpp"
#include "Cardia/Asset/Asset.hpp"


namespace Cardia
{
	enum class TextureMode
	{
		Texture2D,
		CubeMap
	};

	class Renderer;
	class Texture final : public Asset
	{
	public:
		void Reload(const std::filesystem::path& path) override;

		Texture(const Texture& other) = delete;
		Texture& operator=(const Texture& other) = delete;
		Texture(Texture&& other) noexcept;
		Texture& operator=(Texture&& other) noexcept;

		Texture(const Device& device, const std::filesystem::path& path, TextureMode textureMode = TextureMode::Texture2D);
		Texture(
			const Device& device,
			const VkExtent2D& size,
			VkFormat format = VK_FORMAT_R8G8B8A8_SRGB,
			VkImageUsageFlags usageFlags = VK_IMAGE_USAGE_SAMPLED_BIT,
			VkImageAspectFlags aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT);

		Texture(const Device& device, const VkExtent2D& size, const void* data);

		~Texture() override;

		uint32_t GetHeight() const { return m_Size.height; }
		uint32_t GetWidth() const { return m_Size.width; }

		VkSampler GetSampler() const { return m_TextureSampler; }
		VkImageView GetView() const { return m_TextureImageView; }
		VkDescriptorImageInfo GetImageInfo() const;

	private:
		void Init(const std::filesystem::path& path);
		void Release();

		void CreateImage(VkFormat format, VkImageUsageFlags usageFlags, VkImageAspectFlags aspectFlags);
		void TransitionImageLayout(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, VkImageLayout oldLayout, VkImageLayout newLayout) const;
		void CreateImageView(VkFormat format, VkImageAspectFlags aspectFlags);
		void CreateTextureSampler();

		VkExtent2D m_Size {};

		const Device& m_Device;
		VkImage m_TextureImage {};
		VkDeviceMemory m_TextureImageMemory {};
		VkImageView m_TextureImageView {};
		VkSampler m_TextureSampler {};

		uint32_t m_LayerCount { 1 };
		TextureMode m_TextureMode { TextureMode::Texture2D };
	};
}
