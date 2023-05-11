#include "cdpch.hpp"
#include "Cardia/Renderer/RenderContext.hpp"

namespace Cardia
{
	RenderContext::RenderContext(Window& window) : m_Window(window), m_Device(m_Window), m_SwapChain(m_Device, m_Window.GetExtent())
	{
		CreatePipelineLayout();
		CreatePipeline();
		CreateCommandBuffers();
	}

	RenderContext::~RenderContext()
	{
		vkDeviceWaitIdle(m_Device.GetDevice());
		vkDestroyPipelineLayout(m_Device.GetDevice(), m_PipelineLayout, nullptr);
	}

	void RenderContext::Init()
	{
	}

	void RenderContext::DrawFrame()
	{
		uint32_t imageIndex;
		auto result = m_SwapChain.AcquireNextImage(&imageIndex);

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			throw std::runtime_error("Vulkan : Failed to present swap chain image !");
		}

		result = m_SwapChain.SubmitCommandBuffers(&m_CommandBuffers[imageIndex], &imageIndex);
		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Vulkan : Failed to present swap chain image !");
		}
	}

	void RenderContext::CreatePipelineLayout()
	{
		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo {};
		pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutCreateInfo.setLayoutCount = 0;
		pipelineLayoutCreateInfo.pSetLayouts = nullptr;
		pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
		pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;

		if (vkCreatePipelineLayout(m_Device.GetDevice(), &pipelineLayoutCreateInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS)
		{
			throw std::runtime_error("Vulkan : Failed to create pipeline layout");
		}
	}

	void RenderContext::CreatePipeline()
	{
		auto pipelineConfig = Pipeline::DefaultPipelineConfigInfo(m_SwapChain.Width(), m_SwapChain.Height());
		pipelineConfig.renderPass = m_SwapChain.GetRenderPass();
		pipelineConfig.pipelineLayout = m_PipelineLayout;
		m_Pipeline = std::make_unique<Pipeline>(
			m_Device,
			"resources/shaders/simple.vert.spv",
			"resources/shaders/simple.frag.spv",
			pipelineConfig
		);
	}

	void RenderContext::CreateCommandBuffers()
	{
		m_CommandBuffers.resize(m_SwapChain.ImageCount());

		VkCommandBufferAllocateInfo allocateInfo {};
		allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocateInfo.commandPool = m_Device.GetCommandPool();
		allocateInfo.commandBufferCount = static_cast<uint32_t>(m_CommandBuffers.size());

		if (vkAllocateCommandBuffers(m_Device.GetDevice(), &allocateInfo, m_CommandBuffers.data()) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate command buffers!");
		}

		for (int i = 0; i < m_CommandBuffers.size(); i++) {
			VkCommandBufferBeginInfo beginInfo {};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

			if (vkBeginCommandBuffer(m_CommandBuffers[i], &beginInfo) != VK_SUCCESS) {
				throw std::runtime_error("failed to begin recording command buffer!");
			}

			VkRenderPassBeginInfo renderPassInfo{};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassInfo.renderPass = m_SwapChain.GetRenderPass();
			renderPassInfo.framebuffer = m_SwapChain.GetFrameBuffer(i);

			renderPassInfo.renderArea.offset = {0, 0};
			renderPassInfo.renderArea.extent = m_SwapChain.GetSwapChainExtent();

			std::array<VkClearValue, 2> clearValues{};
			clearValues[0].color = {{0.2f, 0.2f, 0.2f, 1.0f}};
			clearValues[1].depthStencil = {1.0f, 0};
			renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
			renderPassInfo.pClearValues = clearValues.data();

			vkCmdBeginRenderPass(m_CommandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

			m_Pipeline->Bind(m_CommandBuffers[i]);
			vkCmdDraw(m_CommandBuffers[i], 3, 1, 0, 0);

			vkCmdEndRenderPass(m_CommandBuffers[i]);
			if (vkEndCommandBuffer(m_CommandBuffers[i]) != VK_SUCCESS) {
				throw std::runtime_error("failed to record command buffer!");
			}
		}
	}
}
