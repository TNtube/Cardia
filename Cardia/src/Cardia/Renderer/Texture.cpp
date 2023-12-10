#include "cdpch.hpp"
#include "Cardia/Renderer/Texture.hpp"

#include <volk.h>

#include <utility>
#include <Cardia/Application.hpp>

#include "Cardia/Renderer/Buffer.hpp"

namespace
{
	bool hasStencilComponent(const VkFormat format) {
		return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
	}
}

namespace Cardia
{
	Texture::Texture(Texture&& other) noexcept
		: m_CreateInfo(other.m_CreateInfo),
		m_Device(other.m_Device), m_Image(other.m_Image),
		m_Memory(other.m_Memory), m_ImageView(other.m_ImageView),
		m_Sampler(other.m_Sampler), m_LayerCount(other.m_LayerCount),
		m_Initialized(other.m_Initialized)
	{
		other.m_Image = VK_NULL_HANDLE;
		other.m_ImageView = VK_NULL_HANDLE;
		other.m_Memory = VK_NULL_HANDLE;
		other.m_Sampler = VK_NULL_HANDLE;
		other.m_LayerCount = 0;
		other.m_Initialized = false;
	}

	Texture::Texture(const Device& device, const TextureCreateInfo createInfo)
		: m_Device(device), m_CreateInfo(createInfo) {}

	Texture::Texture(const Device& device, const TextureCreateInfo createInfo, const Vector2u size)
		: m_Device(device), m_Size(size), m_CreateInfo(createInfo)
	{
		CreateImage(m_CreateInfo.Format, m_CreateInfo.UsageFlags, m_CreateInfo.AspectFlags);
	}

	Texture::~Texture()
	{
		Release();
	}

	Texture::Texture(const Device& device)
		: Texture(device, {}) {}

	Texture& Texture::operator=(Texture &&other) noexcept
	{
		std::swap(*this, other);
		return *this;
	}

	void Texture::SetCreateInfo(const TextureCreateInfo createInfo)
	{
		Release();
		m_CreateInfo = createInfo;
	}

	void Texture::SetData(const std::vector<std::uint8_t>& data, const Vector2u size)
	{
		m_Size = size;
		CreateImage(m_CreateInfo.Format, m_CreateInfo.UsageFlags, m_CreateInfo.AspectFlags);

		const auto imageSize = m_Size.w * m_Size.h * 4;

		Buffer buffer(m_Device, imageSize, 1, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		buffer.UploadData(imageSize, data.data());

		TransitionImageLayout(m_Image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		if (m_CreateInfo.Mode == TextureMode::CubeMap) {
			m_Device.CopyBufferToImageCubemap(buffer.GetBuffer(), m_Image, m_Size.w, m_Size.h, 1);
		} else {
			m_Device.CopyBufferToImage(buffer.GetBuffer(), m_Image, m_Size.w, m_Size.h, 1);
		}
		TransitionImageLayout(m_Image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	}

	void Texture::Release()
	{
		if (!m_Initialized) return;

		m_Device.WaitIdle();
		vkDestroySampler(m_Device.GetDevice(), m_Sampler, nullptr);
		vkDestroyImageView(m_Device.GetDevice(), m_ImageView, nullptr);
		vkDestroyImage(m_Device.GetDevice(), m_Image, nullptr);
		vkFreeMemory(m_Device.GetDevice(), m_Memory, nullptr);

		m_Sampler = VK_NULL_HANDLE;
		m_ImageView = VK_NULL_HANDLE;
		m_Image = VK_NULL_HANDLE;
		m_Memory = VK_NULL_HANDLE;

		m_Initialized = false;
	}

	void Texture::CreateImage(const VkFormat format, const VkImageUsageFlags usageFlags, const VkImageAspectFlags aspectFlags)
	{
		if (m_Initialized)
			Release();

		m_Size.h = m_CreateInfo.Mode == TextureMode::CubeMap ? m_Size.w : m_Size.h;
		m_LayerCount = m_CreateInfo.Mode == TextureMode::CubeMap ? 6 : 1;

		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = m_Size.w;
		imageInfo.extent.height = m_Size.h;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = m_LayerCount;
		imageInfo.format = format;
		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = usageFlags | VK_IMAGE_USAGE_SAMPLED_BIT;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageInfo.flags = m_CreateInfo.Mode == TextureMode::CubeMap ? VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT : 0;

		m_Device.CreateImageWithInfo(imageInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_Image, m_Memory);

		CreateImageView(format, aspectFlags);
		CreateTextureSampler();

		m_Initialized = true;
	}

	void Texture::TransitionImageLayout(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags,
										VkImageLayout oldLayout, VkImageLayout newLayout) const
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
			0,0, nullptr,
			0, nullptr,1,
			&barrier
		);

		m_Device.EndSingleTimeCommands(commandBuffer);
	}

	void Texture::CreateImageView(VkFormat format, VkImageAspectFlags aspectFlags)
	{
		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = m_Image;
		viewInfo.viewType =
			m_CreateInfo.Mode == TextureMode::CubeMap
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
}
