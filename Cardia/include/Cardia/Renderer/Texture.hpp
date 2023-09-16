#pragma once

#include <memory>
#include <string>
#include <utility>

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

	struct TextureCreateInfo
	{
		VkExtent2D Size {1, 1};
		VkFormat Format {VK_FORMAT_R8G8B8A8_SRGB};
		VkImageUsageFlags UsageFlags {VK_IMAGE_USAGE_TRANSFER_DST_BIT};
		VkImageAspectFlags AspectFlags {VK_IMAGE_ASPECT_COLOR_BIT};
		TextureMode TextureMode = TextureMode::Texture2D;
		void* Data = nullptr;
	};

	class Renderer;
	class Texture final : public Asset
	{
	public:
		class Builder
		{
		public:
			explicit Builder(const Device& device) : m_Device(device) {}
			Builder& SetAssetHandle(AssetHandle assetHandle) { m_AssetHandle = std::move(assetHandle); return *this; }
			Builder& SetTextureMode(TextureMode textureMode) { m_TextureCreateInfo.TextureMode = textureMode; return *this; }
			Builder& SetFormat(VkFormat format) { m_TextureCreateInfo.Format = format; return *this; }
			Builder& SetSize(const VkExtent2D& size) { m_TextureCreateInfo.Size = size; return *this; }
			Builder& SetUsageFlags(VkImageUsageFlags usageFlags) { m_TextureCreateInfo.UsageFlags = usageFlags; return *this; }
			Builder& SetAspectFlags(VkImageAspectFlags aspectFlags) { m_TextureCreateInfo.AspectFlags = aspectFlags; return *this; }
			Builder& SetData(void* data) { m_TextureCreateInfo.Data = data; return *this; }
			std::unique_ptr<Texture> Build() const;
		private:
			const Device& m_Device;
			AssetHandle m_AssetHandle;
			TextureCreateInfo m_TextureCreateInfo {};
		};

	public:
		void Reload() override;

		Texture(const Texture& other) = delete;
		Texture& operator=(const Texture& other) = delete;
		Texture(Texture&& other) noexcept;
		Texture& operator=(Texture&& other) noexcept;

		Texture(const Device& device, AssetHandle assetHandle, const TextureCreateInfo& textureCreateInfo);
		Texture(const Device& device, const TextureCreateInfo& textureCreateInfo);

		~Texture() override;

		uint32_t GetHeight() const { return m_CreateInfo.Size.height; }
		uint32_t GetWidth() const { return m_CreateInfo.Size.width; }

		VkSampler GetSampler() const { return m_Sampler; }
		VkImageView GetView() const { return m_ImageView; }
		VkDescriptorImageInfo GetImageInfo() const;

	private:
		void Init();
		void Release();

		void CreateImage(VkFormat format, VkImageUsageFlags usageFlags, VkImageAspectFlags aspectFlags);
		void TransitionImageLayout(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, VkImageLayout oldLayout, VkImageLayout newLayout) const;
		void CreateImageView(VkFormat format, VkImageAspectFlags aspectFlags);
		void CreateTextureSampler();

		TextureCreateInfo m_CreateInfo {};

		const Device& m_Device;
		VkImage m_Image {};
		VkDeviceMemory m_Memory {};
		VkImageView m_ImageView {};
		VkSampler m_Sampler {};

		uint32_t m_LayerCount { 1 };
	};
}
