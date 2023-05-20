#include "cdpch.hpp"
#include "Cardia/Renderer/RenderContext.hpp"

#include <GLFW/glfw3.h>

namespace Cardia
{
	RenderContext::RenderContext(Window& window) : m_Window(window), m_Device(m_Window)
	{
		std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
		auto& sub = mesh->GetSubMeshes().emplace_back();
		
		sub.GetVertices().push_back({{0.0f, -0.5f, 1.0f}, {1.0, 0.0, 0.0}});
		sub.GetVertices().push_back({{0.5f, 0.5f, 1.0f}, {0.0, 1.0, 0.0}});
		sub.GetVertices().push_back({{-0.5f, 0.5f, 1.0f}, {0.0, 0.0, 1.0}});
		m_MeshRenderer.SubmitMesh(m_Device, mesh);

		CreatePipelineLayout();
		RecreateSwapChain();
		CreateCommandBuffers();
	}

	RenderContext::~RenderContext()
	{
		vkDeviceWaitIdle(m_Device.GetDevice());
		vkDestroyPipelineLayout(m_Device.GetDevice(), m_PipelineLayout, nullptr);
	}

	void RenderContext::DrawFrame()
	{
		uint32_t imageIndex;
		auto result = m_SwapChain->AcquireNextImage(&imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			RecreateSwapChain();
			return;
		}
		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			throw std::runtime_error("Vulkan : Failed to present swap chain image !");
		}

		RecordCommandBuffer(imageIndex);
		result = m_SwapChain->SubmitCommandBuffers(&m_CommandBuffers[imageIndex], &imageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_Window.WasResized())
		{
			m_Window.ResetResizedFlag();
			RecreateSwapChain();
			return;
		}
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
		PipelineConfigInfo pipelineConfig = Pipeline::DefaultPipelineConfigInfo(m_SwapChain->Width(), m_SwapChain->Height());
		pipelineConfig.renderPass = m_SwapChain->GetRenderPass();
		pipelineConfig.pipelineLayout = m_PipelineLayout;
		m_Pipeline = std::make_unique<Pipeline>(
			m_Device,
			"resources/shaders/simple.vert.spv",
			"resources/shaders/simple.frag.spv",
			pipelineConfig
		);
	}

	void RenderContext::RecreateSwapChain()
	{
		auto extent = m_Window.GetExtent();
		while (extent.width == 0 || extent.height == 0)
		{
			extent = m_Window.GetExtent();
			glfwWaitEvents();
		}
		vkDeviceWaitIdle(m_Device.GetDevice());
		m_SwapChain.reset();
		m_SwapChain = std::make_unique<SwapChain>(m_Device, extent);
		CreatePipeline();
	}

	void RenderContext::CreateCommandBuffers()
	{
		m_CommandBuffers.resize(m_SwapChain->ImageCount());

		VkCommandBufferAllocateInfo allocateInfo {};
		allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocateInfo.commandPool = m_Device.GetCommandPool();
		allocateInfo.commandBufferCount = static_cast<uint32_t>(m_CommandBuffers.size());

		if (vkAllocateCommandBuffers(m_Device.GetDevice(), &allocateInfo, m_CommandBuffers.data()) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate command buffers!");
		}
	}

	void RenderContext::RecordCommandBuffer(int imageIndex) const
	{
		VkCommandBufferBeginInfo beginInfo {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(m_CommandBuffers[imageIndex], &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording command buffer!");
		}

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = m_SwapChain->GetRenderPass();
		renderPassInfo.framebuffer = m_SwapChain->GetFrameBuffer(imageIndex);

		renderPassInfo.renderArea.offset = {0, 0};
		renderPassInfo.renderArea.extent = m_SwapChain->GetSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = {{0.2f, 0.2f, 0.2f, 1.0f}};
		clearValues[1].depthStencil = {1.0f, 0};
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(m_CommandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		// VkViewport viewport {
		// 	.x = 0.0f,
		// 	.y = 0.0f,
		// 	.width = static_cast<float>(m_SwapChain->GetSwapChainExtent().width),
		// 	.height = static_cast<float>(m_SwapChain->GetSwapChainExtent().height),
		// 	.minDepth = 0.0f,
		// 	.maxDepth = 1.0f
		// };
		//
		// VkRect2D scissor { {0, 0}, m_SwapChain->GetSwapChainExtent() };
		//
		// vkCmdSetViewport(m_CommandBuffers[imageIndex], 0,  1, &viewport);
		// vkCmdSetScissor(m_CommandBuffers[imageIndex], 0,  1, &scissor);

		m_Pipeline->Bind(m_CommandBuffers[imageIndex]);
		m_MeshRenderer.Draw(m_CommandBuffers[imageIndex]);

		vkCmdEndRenderPass(m_CommandBuffers[imageIndex]);
		if (vkEndCommandBuffer(m_CommandBuffers[imageIndex]) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer!");
		}
	}
}
