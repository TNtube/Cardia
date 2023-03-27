#pragma once

#include <memory>
#include <string>
#include <filesystem>
#include <vulkan/vulkan.h>


namespace Cardia
{
	class Texture {
	public:
		virtual ~Texture() = default;
		virtual uint32_t GetHeight() const = 0;
		virtual uint32_t GetWidth() const = 0;

		virtual void Bind(int slot = 0) const = 0;
		virtual bool operator==(const Texture& other) const = 0;
		virtual uint32_t GetRendererID() = 0;
		virtual bool IsLoaded() { return m_Loaded; }
		virtual std::string GetPath() const { return m_Path; }
	protected:
		bool m_Loaded = false;
		std::string m_Path {};
	};

	class Texture2D : public Texture
	{
	public:
		static void CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
		virtual uint32_t GetHeight() const override { return m_Height; }
		virtual uint32_t GetWidth() const override { return m_Width; }

		Texture2D() = default;
		explicit Texture2D(const std::filesystem::path& path);
		virtual ~Texture2D() override;
		
		static std::unique_ptr<Texture2D> create(const std::string& path);
		static std::unique_ptr<Texture2D> create(int width, int height, void* data);

	private:
		uint32_t m_Width {};
		uint32_t m_Height {};
		
		VkImage m_TextureImage {};
		VkDeviceMemory m_TextureImageMemory {};
	};
}
