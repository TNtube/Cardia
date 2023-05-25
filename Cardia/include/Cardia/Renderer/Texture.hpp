#pragma once

#include <memory>
#include <string>
#include <filesystem>
#include <vulkan/vulkan.h>

#include "Device.hpp"


namespace Cardia
{
	class Texture {
	public:
		virtual ~Texture() = default;
		virtual uint32_t GetHeight() const = 0;
		virtual uint32_t GetWidth() const = 0;
		virtual VkSampler GetSampler() const = 0;
	protected:
		bool m_Loaded = false;
		std::string m_Path {};
	};

	class Texture2D : public Texture
	{
	public:
		explicit Texture2D(Device& device, const std::filesystem::path& path);
		virtual ~Texture2D() override;

		virtual uint32_t GetHeight() const override { return m_Height; }
		virtual uint32_t GetWidth() const override { return m_Width; }

		VkSampler GetSampler() const override { return m_TextureSampler; }

		static std::unique_ptr<Texture2D> create(const std::string& path);
		static std::unique_ptr<Texture2D> create(int width, int height, void* data);

	private:
		void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout) const;
		void CreateImageView(VkFormat format, VkImageAspectFlags aspectFlags);
		void CreateTextureSampler();

		uint32_t m_Width {};
		uint32_t m_Height {};
		
		Device& m_Device;
		VkImage m_TextureImage {};
		VkDeviceMemory m_TextureImageMemory {};
		VkImageView m_TextureImageView {};
		VkSampler m_TextureSampler {};
	};
}
