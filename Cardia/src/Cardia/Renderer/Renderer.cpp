#include "cdpch.hpp"
#include "Cardia/Renderer/Renderer.hpp"

#include <GLFW/glfw3.h>
#include <Cardia/Application.hpp>

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

		Shader shader(m_Device);

		shader.AddShader(ShaderType::Vertex, "resources/shaders/simple.vert.spv");
		shader.AddShader(ShaderType::Fragment, "resources/shaders/simple.frag.spv");

		VkPushConstantRange pushConstant;
		pushConstant.offset = 0;
		pushConstant.size = sizeof(PushConstantData);
		pushConstant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

		auto mvpLayout = DescriptorSetLayout::Builder(*m_DescriptorLayoutCache)
			.AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT, 1)
			.Build();

		m_MaterialDescriptorSetLayout = DescriptorSetLayout::Builder(*m_DescriptorLayoutCache)
				.AddBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 1) // Albedo
				.AddBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 1) // Normal
				.AddBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 1) // MetallicRoughness
				.AddBinding(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT, 1) // AO
				.AddBinding(4, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT, 1) // Albedo Color
				.Build();

		const std::array<VkDescriptorSetLayout, 2> descLayouts = {
			mvpLayout->GetDescriptorSetLayout(),
			m_MaterialDescriptorSetLayout->GetDescriptorSetLayout()
		};

		const std::array<VkPushConstantRange, 1> pushConsts = { pushConstant };

		PipelineBuilder builder{ m_Device };
		builder.SetShader(&shader);
		builder.SetInputAssembly(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, false);
		builder.SetViewport(m_SwapChain->GetExtent().width, m_SwapChain->GetExtent().height);
		builder.SetRasterizer(VK_POLYGON_MODE_FILL, VK_CULL_MODE_FRONT_BIT);
		builder.SetMultisampling();
		builder.SetDepthStencil(true, true, VK_COMPARE_OP_LESS);
		builder.SetColorBlend(true, VK_BLEND_OP_ADD, VK_BLEND_OP_ADD, false, VK_LOGIC_OP_COPY);
		builder.SetDescriptorSetLayout(static_cast<uint32_t>(descLayouts.size()), descLayouts.data(), static_cast<uint32_t>(pushConsts.size()), pushConsts.data());

		m_MainPipeline = builder.BuildGraphics(m_SwapChain->GetRenderPass(), Vertex::GetBindingDescriptions(), Vertex::GetAttributeDescriptions());

		for (auto& frame : m_Frames)
		{
			frame.MainUboBuffer = std::make_shared<Buffer>(
				m_Device,
				sizeof(UboData),
				1,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

			auto bufferInfo = frame.MainUboBuffer->DescriptorInfo();

			frame.MainUboDescriptorSet = DescriptorSet::Writer(*m_DescriptorAllocator, *mvpLayout)
				.WriteBuffer(0, &bufferInfo)
				.Build();

			frame.SkyboxUboBuffer = std::make_shared<Buffer>(
				m_Device,
				sizeof(SkyboxUboData),
				1,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

			bufferInfo = frame.SkyboxUboBuffer->DescriptorInfo();

			frame.SkyboxUboDescriptorSet = DescriptorSet::Writer(*m_DescriptorAllocator, *mvpLayout)
				.WriteBuffer(0, &bufferInfo)
				.Build();
		}

		std::uint8_t whiteColor = 0xffffffff;
		std::uint8_t normalColor = 0x8080ffff;
		constexpr Vector2u size {1, 1};

		m_WhiteTexture = std::make_unique<Texture>(m_Device);
		m_WhiteTexture->SetData({whiteColor}, size);

		m_NormalTexture = std::make_unique<Texture>(m_Device);
		m_NormalTexture->SetData({normalColor}, size);
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

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_Window.ShouldInvalidateSwapchain())
		{
			RecreateSwapChain();
			m_Window.SwapchainInvalidated();
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
		m_Device.WaitIdle();

		SwapChain::SwapChainInfo info {};
		info.WindowExtent = extent;
		info.IsVsync = m_Window.IsVSync();
		info.Previous = std::move(m_SwapChain);

		m_SwapChain = std::make_unique<SwapChain>(m_Device, info);
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

	const Skybox &Renderer::GetSkybox()
	{
		if (!m_Skybox) {

			auto& am = Application::Get().GetAssetsManager();

			auto id = am.GetUUIDFromRelative("resources/textures/skybox/lilienstein_skybox.tga");
			m_Skybox = std::make_unique<Skybox>(*this, id);
		}
		return *m_Skybox;
	}

	void Renderer::WaitIdle() const
	{
		m_Device.WaitIdle();
	}
}
