#include "cdpch.hpp"
#include "Cardia/Renderer/Framebuffer.hpp"


namespace Cardia
{

	Framebuffer::Framebuffer(const Device& device, const RenderPass& renderPass, const FramebufferSpecification& spec) : m_Device(device), m_Extent(spec.width, spec.height)
	{
		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.flags = spec.flags;
		framebufferInfo.renderPass = renderPass.GetRenderPass();
		framebufferInfo.attachmentCount = static_cast<uint32_t>(spec.attachments.size());
		framebufferInfo.pAttachments = spec.attachments.data();
		framebufferInfo.width = spec.width;
		framebufferInfo.height = spec.height;
		framebufferInfo.layers = 1;
		if (vkCreateFramebuffer(m_Device.GetDevice(), &framebufferInfo, nullptr, &m_Framebuffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to create framebuffer!");
		}
	}

	Framebuffer::~Framebuffer()
	{
		vkDestroyFramebuffer(m_Device.GetDevice(), m_Framebuffer, nullptr);
	}

	Framebuffer::Framebuffer(Framebuffer&& other) noexcept : m_Device(other.m_Device)
	{
		m_Framebuffer = other.m_Framebuffer;
		m_Extent = other.m_Extent;
		other.m_Framebuffer = VK_NULL_HANDLE;
	}

	Framebuffer& Framebuffer::operator=(Framebuffer&& other) noexcept
	{
		m_Framebuffer = other.m_Framebuffer;
		m_Extent = other.m_Extent;
		other.m_Framebuffer = VK_NULL_HANDLE;
		return *this;
	}
}
