#include "cdpch.hpp"
#include "Cardia/Renderer/Renderer.hpp"

#include <imgui_impl_vulkan.h>
#include <GLFW/glfw3.h>

namespace Cardia
{
	Renderer::Renderer(Window& window)
		: m_Window(window),
		m_Device(m_Window)
	{
		RecreateSwapChain();
		CreateCommandBuffers();
	}

	Renderer::~Renderer()
	{
		vkDeviceWaitIdle(m_Device.GetDevice());
	}

	VkCommandBuffer Renderer::Begin()
	{
		auto result = m_SwapChain->AcquireNextImage(&m_CurrentImageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			RecreateSwapChain();
			return nullptr;
		}
		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			throw std::runtime_error("Vulkan : Failed to present swap chain image !");
		}

		VkCommandBufferBeginInfo beginInfo {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		if (vkBeginCommandBuffer(m_CommandBuffers[m_CurrentImageIndex], &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording command buffer!");
		}

		return m_CommandBuffers[m_CurrentImageIndex];
	}

	void Renderer::End()
	{
		if (vkEndCommandBuffer(m_CommandBuffers[m_CurrentImageIndex]) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer!");
		}
		const auto result = m_SwapChain->SubmitCommandBuffers(&m_CommandBuffers[m_CurrentImageIndex], &m_CurrentImageIndex);
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

	void Renderer::BeginSwapChainRenderPass() const
	{
		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = m_SwapChain->GetRenderPass();
		renderPassInfo.framebuffer = m_SwapChain->GetFrameBuffer(m_CurrentImageIndex);

		renderPassInfo.renderArea.offset = {0, 0};
		renderPassInfo.renderArea.extent = m_SwapChain->GetSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = {{0.2f, 0.2f, 0.2f, 1.0f}};
		clearValues[1].depthStencil = {1.0f, 0};
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(m_CommandBuffers[m_CurrentImageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		const auto extent = m_SwapChain->GetSwapChainExtent();
		VkViewport viewport {};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(extent.width);
		viewport.height = static_cast<float>(extent.height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(m_CommandBuffers[m_CurrentImageIndex], 0, 1, &viewport);

		VkRect2D scissor {};
		scissor.offset = {0, 0};
		scissor.extent = extent;
		vkCmdSetScissor(m_CommandBuffers[m_CurrentImageIndex], 0, 1, &scissor);
	}

	void Renderer::EndSwapChainRenderPass() const
	{
		vkCmdEndRenderPass(m_CommandBuffers[m_CurrentImageIndex]);
	}

	void Renderer::RecreateSwapChain()
	{
		auto extent = m_Window.GetExtent();
		while (extent.width == 0 || extent.height == 0)
		{
			extent = m_Window.GetExtent();
			glfwWaitEvents();
		}
		vkDeviceWaitIdle(m_Device.GetDevice());
		
		if (m_SwapChain == nullptr)
		{
			m_SwapChain = std::make_unique<SwapChain>(m_Device, extent);
		} else
		{
			m_SwapChain = std::make_unique<SwapChain>(m_Device, extent, std::move(m_SwapChain));
			if (m_SwapChain->ImageCount() != m_CommandBuffers.size())
			{
				FreeCommandBuffers();
				CreateCommandBuffers();
			}
		}
	}

	void Renderer::CreateCommandBuffers()
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

	void Renderer::FreeCommandBuffers()
	{
		vkFreeCommandBuffers(
			m_Device.GetDevice(),
			m_Device.GetCommandPool(),
			static_cast<uint32_t>(m_CommandBuffers.size()),
			m_CommandBuffers.data());
		m_CommandBuffers.clear();
	}

	void Renderer::RecordCommandBuffer(uint32_t imageIndex) const
	{
	}
}
