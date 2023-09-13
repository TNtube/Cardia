#pragma once

#include <volk.h>

#include "Device.hpp"

namespace Cardia
{
	struct RenderPassSpecification
	{
		std::vector<VkAttachmentDescription> AttachmentDescriptions;
		VkSubpassDescription SubpassDescription;
		VkSubpassDependency SubpassDependency;
	};

	class RenderPass
	{
	public:
		RenderPass(const Device& device, const RenderPassSpecification& spec);
		virtual ~RenderPass();
		RenderPass(const RenderPass& other) = delete;
		RenderPass& operator=(const RenderPass& other) = delete;


		RenderPass(RenderPass&& other) noexcept
			: m_Device(other.m_Device),
			  m_RenderPass(other.m_RenderPass)
		{
			other.m_RenderPass = VK_NULL_HANDLE;
		}

		RenderPass& operator=(RenderPass&& other) noexcept
		{
			if (this == &other)
				return *this;
			m_RenderPass = other.m_RenderPass;
			other.m_RenderPass = VK_NULL_HANDLE;
			return *this;
		}

		VkRenderPass GetRenderPass() const { return m_RenderPass; }

	private:
		const Device& m_Device;
		VkRenderPass m_RenderPass {};
	};
}
