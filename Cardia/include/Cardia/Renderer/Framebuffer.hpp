#pragma once
#include "Device.hpp"
#include <volk.h>

#include "RenderPass.hpp"


namespace Cardia
{
	struct FramebufferSpecification
	{
		uint32_t width;
		uint32_t height;
		std::vector<VkImageView> attachments;
		VkFramebufferCreateFlags flags = 0;
	};

	class Framebuffer
	{
	public:
		Framebuffer(const Device& device, const RenderPass& renderPass, const FramebufferSpecification& spec);
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
		const Device& m_Device;
		VkFramebuffer m_Framebuffer {};
		VkExtent2D m_Extent {};
	};
}
