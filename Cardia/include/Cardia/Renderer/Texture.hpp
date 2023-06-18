#pragma once

#include <memory>
#include <string>
#include <filesystem>
#include <glm/vec2.hpp>
#include <vulkan/vulkan.h>

#include "Device.hpp"
#include "Descriptors.hpp"
#include "Pipeline.hpp"


namespace Cardia
{

	class Renderer;
	class Texture2D final
	{
	public:
		Texture2D(const Texture2D& other) = delete;
		Texture2D& operator=(const Texture2D& other) = delete;

		Texture2D(Texture2D&& other) noexcept
			: m_Width{other.m_Width},
			  m_Height{other.m_Height},
			  m_Device{other.m_Device},
			  m_Renderer{other.m_Renderer},
			  m_TextureImage{other.m_TextureImage},
			  m_TextureImageMemory{other.m_TextureImageMemory},
			  m_TextureImageView{other.m_TextureImageView},
			  m_TextureSampler{other.m_TextureSampler},
			  m_TextureDescriptorSet{std::move(other.m_TextureDescriptorSet)}
		{
			other.m_TextureImage = VK_NULL_HANDLE;
			other.m_TextureImageMemory = VK_NULL_HANDLE;
			other.m_TextureImageView = VK_NULL_HANDLE;
			other.m_TextureSampler = VK_NULL_HANDLE;
		}

		Texture2D& operator=(Texture2D&& other) noexcept
		{
			using std::swap;
			swap(*this, other);
			return *this;
		}

		Texture2D(Device& device, Renderer& renderer, const std::filesystem::path& path);
		Texture2D(
			Device& device,
			Renderer& renderer,
			const VkExtent2D& size,
			VkFormat format = VK_FORMAT_R8G8B8A8_SRGB,
			VkImageUsageFlags usageFlags = VK_IMAGE_USAGE_SAMPLED_BIT,
			VkImageAspectFlags aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT);
		virtual ~Texture2D();

		uint32_t GetHeight() const { return m_Height; }
		uint32_t GetWidth() const { return m_Width; }

		VkSampler GetSampler() const { return m_TextureSampler; }
		VkImageView GetView() const { return m_TextureImageView; }
		DescriptorSet& GetDescriptorSet() const { return *m_TextureDescriptorSet; }
		
		void Bind(VkCommandBuffer commandBuffer) const;

		static std::unique_ptr<Texture2D> create(const std::string& path);
		static std::unique_ptr<Texture2D> create(int width, int height, void* data);

	private:
		void CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageUsageFlags usageFlags, VkImageAspectFlags aspectFlags);
		void TransitionImageLayout(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, VkImageLayout oldLayout, VkImageLayout newLayout) const;
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
