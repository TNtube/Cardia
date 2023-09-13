#include "cdpch.hpp"

#include "Cardia/Renderer/RenderPass.hpp"

namespace Cardia
{
	RenderPass::RenderPass(const Device& device, const RenderPassSpecification& spec) : m_Device(device)
	{
		VkRenderPassCreateInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = static_cast<uint32_t>(spec.AttachmentDescriptions.size());
		renderPassInfo.pAttachments = spec.AttachmentDescriptions.data();
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &spec.SubpassDescription;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &spec.SubpassDependency;

		if (vkCreateRenderPass(m_Device.GetDevice(), &renderPassInfo, nullptr, &m_RenderPass) != VK_SUCCESS) {
			throw std::runtime_error("failed to create render pass!");
		}
	}

	RenderPass::~RenderPass()
	{
		vkDestroyRenderPass(m_Device.GetDevice(), m_RenderPass, nullptr);
	}
}
