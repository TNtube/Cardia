#pragma once
#include "Device.hpp"
#include <vulkan/vulkan.h>


namespace Cardia
{
	struct FramebufferSpecification
	{
		uint32_t width;
		uint32_t height;
		std::vector<VkImageView> attachments;
	};

	class Framebuffer
	{
	public:
		Framebuffer(Device& device, const VkRenderPass& renderPass, const FramebufferSpecification& spec);
		virtual ~Framebuffer();
		Framebuffer(const Framebuffer &) = delete;
		Framebuffer& operator=(const Framebuffer &) = delete;
		Framebuffer(Framebuffer && other) noexcept;
		Framebuffer& operator=(Framebuffer&& other) noexcept;

		const VkFramebuffer& GetFramebuffer() const { return m_Framebuffer; }
		// void Bind() const;
		// void Unbind() const;
		//
		// void Resize(int width, int height);
		// int ReadPixel(uint32_t attachmentIndex, int x, int y);
		//
		// void ClearAttachment(uint32_t attachmentIndex, int value);
		//
		// uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const;
		
	private:
		Device& m_Device;
		VkFramebuffer m_Framebuffer {};

	};
}
