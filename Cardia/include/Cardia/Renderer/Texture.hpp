#pragma once

#include <memory>
#include <string>
#include <utility>

#include "Device.hpp"
#include "Descriptors.hpp"
#include "Cardia/Assets/Asset.hpp"


namespace Cardia
{
	enum class TextureMode
	{
		Texture2D,
		CubeMap
	};

	struct TextureCreateInfo
	{
		VkFormat Format {VK_FORMAT_R8G8B8A8_UNORM};
		VkImageUsageFlags UsageFlags {VK_IMAGE_USAGE_TRANSFER_DST_BIT};
		VkImageAspectFlags AspectFlags {VK_IMAGE_ASPECT_COLOR_BIT};
		TextureMode Mode = TextureMode::Texture2D;

		constexpr static auto properties = std::make_tuple(
			property(&TextureCreateInfo::Format, "Format"),
			property(&TextureCreateInfo::UsageFlags, "UsageFlags"),
			property(&TextureCreateInfo::AspectFlags, "AspectFlags"),
			property(&TextureCreateInfo::Mode, "Mode")
		);
	};

	class Renderer;
	class Texture
	{
	public:
		Texture(const Texture& other) = delete;
		Texture& operator=(const Texture& other) = delete;
		Texture(Texture&& other) noexcept;
		Texture& operator=(Texture&& other) noexcept;
		virtual ~Texture();

		explicit Texture(const Device& device);
		Texture(const Device& device, TextureCreateInfo createInfo);
		Texture(const Device& device, TextureCreateInfo createInfo, Vector2u size);
		bool Valid() const { return m_Initialized; }

		void SetCreateInfo(TextureCreateInfo createInfo);
		void SetData(const std::vector<std::uint8_t>& data, Vector2u size);

		uint32_t GetWidth() const { return m_Size.w; }
		uint32_t GetHeight() const { return m_Size.h; }

		VkSampler GetSampler() const { return m_Sampler; }
		VkImageView GetView() const { return m_ImageView; }
		VkDescriptorImageInfo GetImageInfo() const;

	private:
		void Release();

		void CreateImage(VkFormat format, VkImageUsageFlags usageFlags, VkImageAspectFlags aspectFlags);
		void TransitionImageLayout(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, VkImageLayout oldLayout, VkImageLayout newLayout) const;
		void CreateImageView(VkFormat format, VkImageAspectFlags aspectFlags);
		void CreateTextureSampler();

		const Device& m_Device;

		Vector2u m_Size {1, 1};
		bool m_Initialized = false;

		TextureCreateInfo m_CreateInfo {};

		VkImage m_Image {};
		VkDeviceMemory m_Memory {};
		VkImageView m_ImageView {};
		VkSampler m_Sampler {};

		uint32_t m_LayerCount { 1 };
	};
}
