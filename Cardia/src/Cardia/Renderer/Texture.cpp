#include "cdpch.hpp"
#include "Cardia/Renderer/Texture.hpp"

#include <volk.h>

#include "Cardia/Renderer/Buffer.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"
#include "Cardia/Renderer/Renderer.hpp"

namespace
{
	bool hasStencilComponent(VkFormat format) {
		return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
	}
}

namespace Cardia
{
	void Texture::Reload()
	{
		if (m_Handle.Path.empty())
			return;
		Release();
		Init();
	}

	Texture &Texture::operator=(Texture &&other) noexcept
	{
		std::swap(*this, other);
		return *this;
	}

	Texture::Texture(Texture &&other) noexcept
		: Asset(std::move(other.m_Handle)),
		  m_Device{other.m_Device},
		  m_Image{other.m_Image},
		  m_Memory{other.m_Memory},
		  m_ImageView{other.m_ImageView},
		  m_Sampler{other.m_Sampler},
		  m_LayerCount{other.m_LayerCount},
		  m_CreateInfo{other.m_CreateInfo}
	{
		other.m_Image = VK_NULL_HANDLE;
		other.m_Memory = VK_NULL_HANDLE;
		other.m_ImageView = VK_NULL_HANDLE;
		other.m_Sampler = VK_NULL_HANDLE;
	}

	Texture::Texture(const Device& device, AssetHandle assetHandle, const TextureCreateInfo& textureCreateInfo)
		: Asset(std::move(assetHandle)), m_Device(device), m_CreateInfo(textureCreateInfo)
	{
		Init();
	}

	Texture::Texture(const Device& device, const TextureCreateInfo& textureCreateInfo)
		: m_Device(device), m_CreateInfo(textureCreateInfo)
	{
		CreateImage(m_CreateInfo.Format, m_CreateInfo.UsageFlags, m_CreateInfo.AspectFlags);

		if (m_CreateInfo.Data) {
			CreateImage(VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_USAGE_TRANSFER_DST_BIT, VK_IMAGE_ASPECT_COLOR_BIT);

			const auto imageSize = m_CreateInfo.Size.width * m_CreateInfo.Size.height * 4;
			Buffer buffer(device, imageSize, 1, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
			buffer.UploadData(imageSize, m_CreateInfo.Data);

			TransitionImageLayout(m_Image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
			if (m_CreateInfo.TextureMode == TextureMode::CubeMap) {
				device.CopyBufferToImageCubemap(buffer.GetBuffer(), m_Image, m_CreateInfo.Size.width, m_CreateInfo.Size.height, 1);
			} else {
				device.CopyBufferToImage(buffer.GetBuffer(), m_Image, m_CreateInfo.Size.width, m_CreateInfo.Size.height, 1);
			}
			TransitionImageLayout(m_Image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

			// We are not supposed to use that anymore, it is not managed by the texture
			m_CreateInfo.Data = nullptr;
		}
	}

	Texture::~Texture()
	{
		Release();
	}

//	void SpriteTexture::Bind(VkCommandBuffer commandBuffer) const
//	{
//		vkCmdBindDescriptorSets(
//			commandBuffer,
//			VK_PIPELINE_BIND_POINT_GRAPHICS,
//			m_Renderer.GetPipelineLayout().GetPipelineLayout(),
//			1, 1,
//			&m_TextureDescriptorSet->GetDescriptor(),
//			0, nullptr);
//	}

	void Texture::Init()
	{
		int texWidth {}, texHeight {}, texChannels {};
		stbi_uc* pixels = stbi_load(m_Handle.Path.string().c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

		unsigned char errorColor[] = {255, 0, 255, 255};
		if (!pixels)
		{
			Log::CoreError(stbi_failure_reason());
			texWidth = 1;
			texHeight = 1;
			pixels = errorColor;
		}
		m_CreateInfo.Size = {static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight)};
		CreateImage(VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_USAGE_TRANSFER_DST_BIT, VK_IMAGE_ASPECT_COLOR_BIT);

		const auto imageSize = static_cast<uint32_t>(texWidth * texHeight * 4);
		Buffer buffer(m_Device, imageSize, 1, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		buffer.UploadData(imageSize, pixels);
		if (memcmp(pixels, errorColor, sizeof(errorColor)) != 0)
			stbi_image_free(pixels);

		TransitionImageLayout(m_Image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		if (m_CreateInfo.TextureMode == TextureMode::CubeMap) {
			m_Device.CopyBufferToImageCubemap(buffer.GetBuffer(), m_Image, m_CreateInfo.Size.width, m_CreateInfo.Size.height, 1);
		} else {
			m_Device.CopyBufferToImage(buffer.GetBuffer(), m_Image, m_CreateInfo.Size.width, m_CreateInfo.Size.height, 1);
		}
		TransitionImageLayout(m_Image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	}

	void Texture::Release()
	{
		vkDestroySampler(m_Device.GetDevice(), m_Sampler, nullptr);
		vkDestroyImageView(m_Device.GetDevice(), m_ImageView, nullptr);
		vkDestroyImage(m_Device.GetDevice(), m_Image, nullptr);
		vkFreeMemory(m_Device.GetDevice(), m_Memory, nullptr);
	}

	void Texture::CreateImage(VkFormat format, VkImageUsageFlags usageFlags, VkImageAspectFlags aspectFlags)
	{
		m_CreateInfo.Size.height =
			m_CreateInfo.TextureMode == TextureMode::CubeMap
			? m_CreateInfo.Size.width
			: m_CreateInfo.Size.height;

		m_LayerCount = m_CreateInfo.TextureMode == TextureMode::CubeMap ? 6 : 1;
		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = m_CreateInfo.Size.width;
		imageInfo.extent.height = m_CreateInfo.Size.height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = m_LayerCount;
		imageInfo.format = format;
		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = usageFlags | VK_IMAGE_USAGE_SAMPLED_BIT;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageInfo.flags = m_CreateInfo.TextureMode == TextureMode::CubeMap ? VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT : 0;

		m_Device.CreateImageWithInfo(imageInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_Image, m_Memory);

		CreateImageView(format, aspectFlags);
		CreateTextureSampler();

//		VkDescriptorImageInfo imageBufferInfo;
//		imageBufferInfo.sampler = m_Sampler;
//		imageBufferInfo.imageView = m_ImageView;
//		imageBufferInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
//
//		auto& textureLayout = DescriptorSetLayout::Builder(m_Renderer.GetDescriptorLayoutCache())
//			.AddBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
//			.Build();
//
//		m_TextureDescriptorSet = std::make_unique<DescriptorSet>(
//			*DescriptorSet::Writer(m_Renderer.GetDescriptorAllocator(), textureLayout)
//					.WriteImage(0, &imageBufferInfo)
//					.Build());
	}

	void Texture::TransitionImageLayout(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, VkImageLayout oldLayout,
	                                    VkImageLayout newLayout) const
	{
		VkCommandBuffer commandBuffer = m_Device.BeginSingleTimeCommands();

		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = image;
		barrier.subresourceRange.aspectMask = aspectFlags;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = m_LayerCount;

		if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

			if (hasStencilComponent(format)) {
				barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
			}
		}

		VkPipelineStageFlags sourceStage;
		VkPipelineStageFlags destinationStage;

		if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		} else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		} else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		} else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
		{
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		} else {
			throw std::invalid_argument("Vulkan : unsupported layout transition !");
		}

		vkCmdPipelineBarrier(
			commandBuffer,
			sourceStage, destinationStage,
			0,
			0, nullptr,
			0, nullptr,
			1, &barrier
		);

		m_Device.EndSingleTimeCommands(commandBuffer);
	}

	void Texture::CreateImageView(VkFormat format, VkImageAspectFlags aspectFlags)
	{
		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = m_Image;
		viewInfo.viewType =
			m_CreateInfo.TextureMode == TextureMode::CubeMap
			? VK_IMAGE_VIEW_TYPE_CUBE
			: VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = format;
		viewInfo.subresourceRange.aspectMask = aspectFlags;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = m_LayerCount;

		if (vkCreateImageView(m_Device.GetDevice(), &viewInfo, nullptr, &m_ImageView) != VK_SUCCESS) {
			throw std::runtime_error("failed to create texture image view!");
		}
	}

	void Texture::CreateTextureSampler() {
		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	
		samplerInfo.anisotropyEnable = VK_TRUE;
		samplerInfo.maxAnisotropy = m_Device.Properties.limits.maxSamplerAnisotropy;
	
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
	
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = 0.0f;
	
		if (vkCreateSampler(m_Device.GetDevice(), &samplerInfo, nullptr, &m_Sampler) != VK_SUCCESS)
			throw std::runtime_error("failed to create texture sampler!");
	}

	VkDescriptorImageInfo Texture::GetImageInfo() const
	{
		VkDescriptorImageInfo imageBufferInfo;
		imageBufferInfo.sampler = m_Sampler;
		imageBufferInfo.imageView = m_ImageView;
		imageBufferInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		return imageBufferInfo;
	}

	std::unique_ptr<Texture> Texture::Builder::Build() const
	{
		if (m_AssetHandle.IsValid())
			return std::make_unique<Texture>(m_Device, m_AssetHandle, m_TextureCreateInfo);

		return std::make_unique<Texture>(m_Device, m_TextureCreateInfo);
	}
}
