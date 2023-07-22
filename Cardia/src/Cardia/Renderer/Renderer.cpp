#include "cdpch.hpp"
#include "Cardia/Renderer/Renderer.hpp"

#include <GLFW/glfw3.h>

namespace Cardia
{
	Renderer::Renderer(Window& window)
		: m_Window(window),
		m_Device(m_Window)
	{
		RecreateSwapChain();
		CreateCommandBuffers();
		CreateSyncObjects();
		m_DescriptorAllocator = std::make_unique<DescriptorAllocator>(m_Device);

		m_DescriptorLayoutCache = std::make_unique<DescriptorLayoutCache>(m_Device);

		auto& uboSetLayout = DescriptorSetLayout::Builder(*m_DescriptorLayoutCache)
			.AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
			.Build();

		auto& textureLayout = DescriptorSetLayout::Builder(*m_DescriptorLayoutCache)
			.AddBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.Build();

		std::vector descriptorSetLayouts {
			uboSetLayout.GetDescriptorSetLayout(),
			textureLayout.GetDescriptorSetLayout()
		};

		VkPushConstantRange pushConstant;
		pushConstant.offset = 0;
		pushConstant.size = sizeof(PushConstantData);
		pushConstant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

		std::vector pushConstantRanges {
			pushConstant
		};

		m_PipelineLayout = std::make_unique<PipelineLayout>(m_Device, descriptorSetLayouts, pushConstantRanges);
		
		PipelineConfigInfo pipelineConfig = Pipeline::DefaultPipelineConfigInfo(m_SwapChain->Width(), m_SwapChain->Height());
		pipelineConfig.renderPass = m_SwapChain->GetRenderPass().GetRenderPass();
		pipelineConfig.pipelineLayout = m_PipelineLayout->GetPipelineLayout();
		m_Pipeline = std::make_unique<Pipeline>(
			m_Device,
			"resources/shaders/simple.vert.spv",
			"resources/shaders/simple.frag.spv",
			pipelineConfig
		);

		for (auto& frame : m_Frames)
		{
			frame.UboBuffer = std::make_shared<Buffer>(
				m_Device,
				sizeof(UboData),
				1,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

			auto bufferInfo = frame.UboBuffer->DescriptorInfo();

			frame.UboDescriptorSet = std::make_unique<DescriptorSet>(
				*DescriptorSet::Writer(*m_DescriptorAllocator, uboSetLayout)
					.WriteBuffer(0, &bufferInfo)
					.Build());
		}

		uint32_t whiteColor = 0xffffffff;
		constexpr VkExtent2D size {1, 1};
		m_WhiteTexture = std::make_unique<Texture2D>(m_Device, *this, size, &whiteColor);
	}

	Renderer::~Renderer()
	{
		// cleanup synchronization objects
		for (const auto& frame: m_Frames) {
			vkDestroySemaphore(m_Device.GetDevice(), frame.RenderSemaphore, nullptr);
			vkDestroySemaphore(m_Device.GetDevice(), frame.PresentSemaphore, nullptr);
			vkDestroyFence(m_Device.GetDevice(), frame.RenderFence, nullptr);
		}
	}

	VkCommandBuffer Renderer::Begin()
	{
		m_CurrentFrameNumber++;
		const auto& frame = GetCurrentFrame();
		const auto result = m_SwapChain->AcquireNextImage(frame, &m_CurrentImageIndex);

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

		if (vkBeginCommandBuffer(frame.MainCommandBuffer, &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording command buffer!");
		}

		return frame.MainCommandBuffer;
	}

	bool Renderer::End()
	{
		const auto& frame = GetCurrentFrame();
		if (vkEndCommandBuffer(frame.MainCommandBuffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer!");
		}
		const auto result = m_SwapChain->SubmitCommandBuffers(frame, &m_CurrentImageIndex);

		m_Window.UpdateFullscreenMode();

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_Window.WasResized())
		{
			m_Window.ResetResizedFlag();
			RecreateSwapChain();
			return true;
		}
		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Vulkan : Failed to present swap chain image !");
		}

		return false;
	}

	void Renderer::BeginRenderPass(const Framebuffer& frameBuffer, const RenderPass& renderPass) const
	{
		const auto& frame = GetCurrentFrame();
		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = renderPass.GetRenderPass();
		renderPassInfo.framebuffer = frameBuffer.GetFramebuffer();

		renderPassInfo.renderArea.offset = {0, 0};
		renderPassInfo.renderArea.extent = m_SwapChain->GetExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = {{0.2f, 0.2f, 0.2f, 1.0f}};
		clearValues[1].depthStencil = {1.0f, 0};
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(frame.MainCommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		const auto extent = m_SwapChain->GetExtent();
		VkViewport viewport {};
		viewport.x = 0.0f;
		viewport.y = static_cast<float>(extent.height);
		viewport.width = static_cast<float>(extent.width);
		viewport.height = -static_cast<float>(extent.height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(frame.MainCommandBuffer, 0, 1, &viewport);

		VkRect2D scissor {};
		scissor.offset = {0, 0};
		scissor.extent = extent;
		vkCmdSetScissor(frame.MainCommandBuffer, 0, 1, &scissor);
	}

	void Renderer::BeginSwapChainRenderPass() const
	{
		BeginRenderPass(m_SwapChain->GetFrameBuffer(m_CurrentImageIndex), m_SwapChain->GetRenderPass());
	}

	void Renderer::EndRenderPass() const
	{
		vkCmdEndRenderPass(GetCurrentFrame().MainCommandBuffer);
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
		}
	}

	void Renderer::CreateCommandBuffers()
	{
		VkCommandBufferAllocateInfo allocateInfo {};
		allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocateInfo.commandPool = m_Device.GetCommandPool();
		allocateInfo.commandBufferCount = 1;

		for (auto& frame: m_Frames)
		{
			if (vkAllocateCommandBuffers(m_Device.GetDevice(), &allocateInfo, &frame.MainCommandBuffer) != VK_SUCCESS) {
				throw std::runtime_error("failed to allocate command buffers!");
			}
		}
	}

	void Renderer::CreateSyncObjects()
	{
		VkSemaphoreCreateInfo semaphoreInfo = {};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo = {};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (auto& frame: m_Frames)
		{
			if (vkCreateSemaphore(m_Device.GetDevice(), &semaphoreInfo, nullptr, &frame.PresentSemaphore) != VK_SUCCESS ||
				vkCreateSemaphore(m_Device.GetDevice(), &semaphoreInfo, nullptr, &frame.RenderSemaphore) != VK_SUCCESS ||
				vkCreateFence(m_Device.GetDevice(), &fenceInfo, nullptr, &frame.RenderFence) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to create synchronization objects for a frame!");
			}
		}
	}
}
