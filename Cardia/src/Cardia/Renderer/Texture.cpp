#include "cdpch.hpp"
#include "Cardia/Renderer/Texture.hpp"
#include "Cardia/Renderer/Renderer.hpp"

#include <vulkan/vulkan.h>

#include "stb_image/stb_image.h"

namespace Cardia
{
	// Texture2D::Texture2D(const Device& device, const std::filesystem::path& path)
	// {
	// 	int texWidth, texHeight, texChannels;
	// 	stbi_uc* pixels = stbi_load(path.string().c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
	//
	// 	if (!pixels)
	// 		throw std::runtime_error("stbi : failed to load texture image !");
	//
	// 	const auto imageSize = static_cast<uint32_t>(texWidth * texHeight * 4);
	//
	// 	const Buffer buffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	// 	buffer.UploadData(imageSize, pixels);
	//
	// 	stbi_image_free(pixels);
	//
	// 	RenderContext::Instance().CreateImage(texWidth, texHeight, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_TextureImage, m_TextureImageMemory);
	//
	// 	RenderContext::Instance().TransitionImageLayout(m_TextureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	// 	CopyBufferToImage(buffer.GetBuffer(), static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
	// 	RenderContext::Instance().TransitionImageLayout(m_TextureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	//
	// 	m_TextureImageView = RenderContext::Instance().CreateImageView(m_TextureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);
	// 	
	// }
	//
	// Texture2D::~Texture2D()
	// {
	// 	const auto& device = RenderContext::Instance().GetDevice();
	// 	
	// 	vkDestroySampler(device, m_TextureSampler, nullptr);
	// 	vkDestroyImageView(device, m_TextureImageView, nullptr);
	// 	vkDestroyImage(device, m_TextureImage, nullptr);
	// 	vkFreeMemory(device, m_TextureImageMemory, nullptr);
	// }
	//
	// VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags)
	// {
	// 	VkImageViewCreateInfo viewInfo{};
	// 	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	// 	viewInfo.image = image;
	// 	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	// 	viewInfo.format = format;
	// 	viewInfo.subresourceRange.aspectMask = aspectFlags;
	// 	viewInfo.subresourceRange.baseMipLevel = 0;
	// 	viewInfo.subresourceRange.levelCount = 1;
	// 	viewInfo.subresourceRange.baseArrayLayer = 0;
	// 	viewInfo.subresourceRange.layerCount = 1;
	//
	// 	VkImageView imageView;
	// 	if (vkCreateImageView(RenderContext::Instance().GetDevice(), &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
	// 		throw std::runtime_error("failed to create texture image view!");
	// 	}
	//
	// 	return imageView;
	// }
	//
	// void Texture2D::CopyBufferToImage(VkBuffer buffer, uint32_t width, uint32_t height) const
	// {
	// 	const SingleTimeCommand singleTimeCommand;
	//
	// 	VkBufferImageCopy region;
	// 	region.bufferOffset = 0;
	// 	region.bufferRowLength = 0;
	// 	region.bufferImageHeight = 0;
	//
	// 	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	// 	region.imageSubresource.mipLevel = 0;
	// 	region.imageSubresource.baseArrayLayer = 0;
	// 	region.imageSubresource.layerCount = 1;
	//
	// 	region.imageOffset = {0, 0, 0};
	// 	region.imageExtent = {
	// 		width,
	// 		height,
	// 		1
	// 	};
	//
	// 	vkCmdCopyBufferToImage(
	// 		singleTimeCommand.GetCommandBuffer(),
	// 		buffer,
	// 		m_TextureImage,
	// 		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
	// 		1,
	// 		&region
	// 	);
	// }
	//
	// void Texture2D::CreateTextureSampler() {
	// 	VkSamplerCreateInfo samplerInfo{};
	// 	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	// 	samplerInfo.magFilter = VK_FILTER_LINEAR;
	// 	samplerInfo.minFilter = VK_FILTER_LINEAR;
	// 	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	// 	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	// 	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	//
	// 	samplerInfo.anisotropyEnable = VK_TRUE;
	// 	VkPhysicalDeviceProperties properties{};
	// 	vkGetPhysicalDeviceProperties(RenderContext::Instance().GetPhysicalDevice(), &properties);
	// 	samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
	//
	// 	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	// 	samplerInfo.unnormalizedCoordinates = VK_FALSE;
	//
	// 	samplerInfo.compareEnable = VK_FALSE;
	// 	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	//
	// 	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	// 	samplerInfo.mipLodBias = 0.0f;
	// 	samplerInfo.minLod = 0.0f;
	// 	samplerInfo.maxLod = 0.0f;
	//
	// 	if (vkCreateSampler(RenderContext::Instance().GetDevice(), &samplerInfo, nullptr, &m_TextureSampler) != VK_SUCCESS)
	// 		throw std::runtime_error("failed to create texture sampler!");
	// }

	// Old Way
	std::unique_ptr<Texture2D> Texture2D::create(const std::string &path)
	{
		return nullptr;
	}

	std::unique_ptr<Texture2D> Texture2D::create(int width, int height, void* data)
	{
		return nullptr;
	}
}
