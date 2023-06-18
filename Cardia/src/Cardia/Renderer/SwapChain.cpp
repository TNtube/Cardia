#include "Cardia/Renderer/SwapChain.hpp"

// std
#include <array>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <limits>
#include <set>
#include <stdexcept>

#include "Cardia/Renderer/Renderer.hpp"

namespace Cardia
{
	SwapChain::SwapChain(Device &deviceRef, VkExtent2D extent)
			: m_Device{deviceRef}, m_WindowExtent{extent} {
		Init();
	}

	SwapChain::SwapChain(Device &deviceRef, VkExtent2D extent, std::shared_ptr<SwapChain> previous)
			: m_Device{deviceRef}, m_WindowExtent{extent}, m_PreviousSwapChain{std::move(previous)} {
		Init();
		m_PreviousSwapChain = nullptr;
	}

	void SwapChain::Init() {
		CreateSwapChain();
		CreateImageViews();
		CreateRenderPass();
		CreateDepthResources();
		CreateFramebuffers();
	}

	SwapChain::~SwapChain() {
		for (const auto imageView : m_SwapChainImageViews) {
			vkDestroyImageView(m_Device.GetDevice(), imageView, nullptr);
		}
		m_SwapChainImageViews.clear();

		if (m_SwapChain != nullptr) {
			vkDestroySwapchainKHR(m_Device.GetDevice(), m_SwapChain, nullptr);
			m_SwapChain = nullptr;
		}

		for (int i = 0; i < m_DepthImages.size(); i++) {
			vkDestroyImageView(m_Device.GetDevice(), m_DepthImageViews[i], nullptr);
			vkDestroyImage(m_Device.GetDevice(), m_DepthImages[i], nullptr);
			vkFreeMemory(m_Device.GetDevice(), m_DepthImageMemories[i], nullptr);
		}

	}

	VkResult SwapChain::AcquireNextImage(const FrameData &frame, uint32_t *imageIndex) const
	{
		vkWaitForFences(
				m_Device.GetDevice(),
				1,
				&frame.RenderFence,
				VK_TRUE,
				std::numeric_limits<uint64_t>::max());
		vkResetFences(m_Device.GetDevice(), 1, &frame.RenderFence);

		const VkResult result = vkAcquireNextImageKHR(
				m_Device.GetDevice(),
				m_SwapChain,
				std::numeric_limits<uint64_t>::max(),
				frame.PresentSemaphore,	// must be a not signaled semaphore
				VK_NULL_HANDLE,
				imageIndex);

		return result;
	}

	VkResult SwapChain::SubmitCommandBuffers(const FrameData& frame, const uint32_t *imageIndex) const
	{
		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = {frame.PresentSemaphore};
		VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &frame.MainCommandBuffer;

		VkSemaphore signalSemaphores[] = {frame.RenderSemaphore};
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		if (vkQueueSubmit(m_Device.GraphicsQueue(), 1, &submitInfo, frame.RenderFence) != VK_SUCCESS) {
			throw std::runtime_error("failed to submit draw command buffer!");
		}

		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		VkSwapchainKHR swapChains[] = {m_SwapChain};
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;

		presentInfo.pImageIndices = imageIndex;

		const auto result = vkQueuePresentKHR(m_Device.PresentQueue(), &presentInfo);

		return result;
	}

	void SwapChain::CreateSwapChain() {
		SwapChainSupportDetails swapChainSupport = m_Device.GetSwapChainSupport();

		const VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
		const VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
		const VkExtent2D extent = ChooseSwapExtent(swapChainSupport.capabilities);

		uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
		if (swapChainSupport.capabilities.maxImageCount > 0 &&
				imageCount > swapChainSupport.capabilities.maxImageCount) {
			imageCount = swapChainSupport.capabilities.maxImageCount;
		}

		VkSwapchainCreateInfoKHR createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = m_Device.Surface();

		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		QueueFamilyIndices indices = m_Device.FindPhysicalQueueFamilies();
		uint32_t queueFamilyIndices[] = {indices.GraphicsFamily, indices.PresentFamily};

		if (indices.GraphicsFamily != indices.PresentFamily) {
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		} else {
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0;			// Optional
			createInfo.pQueueFamilyIndices = nullptr;	// Optional
		}

		createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;

		createInfo.oldSwapchain = m_PreviousSwapChain == nullptr ? VK_NULL_HANDLE : m_PreviousSwapChain->m_SwapChain;

		if (vkCreateSwapchainKHR(m_Device.GetDevice(), &createInfo, nullptr, &m_SwapChain) != VK_SUCCESS) {
			throw std::runtime_error("failed to create swap chain!");
		}

		// we only specified a minimum number of images in the swap chain, so the implementation is
		// allowed to create a swap chain with more. That's why we'll first query the final number of
		// images with vkGetSwapchainImagesKHR, then resize the container and finally call it again to
		// retrieve the handles.
		vkGetSwapchainImagesKHR(m_Device.GetDevice(), m_SwapChain, &imageCount, nullptr);
		m_SwapChainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(m_Device.GetDevice(), m_SwapChain, &imageCount, m_SwapChainImages.data());

		m_SwapChainImageFormat = surfaceFormat.format;
		m_SwapChainExtent = extent;
	}

	void SwapChain::CreateImageViews() {
		m_SwapChainImageViews.resize(m_SwapChainImages.size());
		for (size_t i = 0; i < m_SwapChainImages.size(); i++) {
			VkImageViewCreateInfo viewInfo{};
			viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			viewInfo.image = m_SwapChainImages[i];
			viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			viewInfo.format = m_SwapChainImageFormat;
			viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			viewInfo.subresourceRange.baseMipLevel = 0;
			viewInfo.subresourceRange.levelCount = 1;
			viewInfo.subresourceRange.baseArrayLayer = 0;
			viewInfo.subresourceRange.layerCount = 1;

			if (vkCreateImageView(m_Device.GetDevice(), &viewInfo, nullptr, &m_SwapChainImageViews[i]) !=
					VK_SUCCESS) {
				throw std::runtime_error("failed to create texture image view!");
			}
		}
	}

	void SwapChain::CreateRenderPass() {
		VkAttachmentDescription depthAttachment{};
		depthAttachment.format = FindDepthFormat();
		depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentReference depthAttachmentRef;
		depthAttachmentRef.attachment = 1;
		depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentDescription colorAttachment = {};
		colorAttachment.format = GetSwapChainImageFormat();
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorAttachmentRef;
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;
		subpass.pDepthStencilAttachment = &depthAttachmentRef;

		VkSubpassDependency dependency = {};

		dependency.dstSubpass = 0;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.srcAccessMask = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

		RenderPassSpecification specification {
			.AttachmentDescriptions = {colorAttachment, depthAttachment},
			.SubpassDescription = subpass,
			.SubpassDependency = dependency
		};

		m_RenderPass = std::make_unique<RenderPass>(m_Device, specification);
	}

	void SwapChain::CreateFramebuffers()
	{
		m_SwapChainFramebuffers.reserve(ImageCount());
		for (size_t i = 0; i < ImageCount(); i++) {
			const std::vector attachments = {m_SwapChainImageViews[i], m_DepthImageViews[i]};
			FramebufferSpecification specification {
				.width = GetSwapChainExtent().width,
				.height = GetSwapChainExtent().height,
				.attachments = attachments
			};
			m_SwapChainFramebuffers.emplace_back(m_Device, *m_RenderPass, specification);
		}
	}

	void SwapChain::CreateDepthResources() {
		VkFormat depthFormat = FindDepthFormat();
		VkExtent2D swapChainExtent = GetSwapChainExtent();

		m_DepthImages.resize(ImageCount());
		m_DepthImageMemories.resize(ImageCount());
		m_DepthImageViews.resize(ImageCount());

		for (int i = 0; i < m_DepthImages.size(); i++) {
			VkImageCreateInfo imageInfo{};
			imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			imageInfo.imageType = VK_IMAGE_TYPE_2D;
			imageInfo.extent.width = swapChainExtent.width;
			imageInfo.extent.height = swapChainExtent.height;
			imageInfo.extent.depth = 1;
			imageInfo.mipLevels = 1;
			imageInfo.arrayLayers = 1;
			imageInfo.format = depthFormat;
			imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
			imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
			imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
			imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			imageInfo.flags = 0;

			m_Device.CreateImageWithInfo(
					imageInfo,
					VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
					m_DepthImages[i],
					m_DepthImageMemories[i]);

			VkImageViewCreateInfo viewInfo{};
			viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			viewInfo.image = m_DepthImages[i];
			viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			viewInfo.format = depthFormat;
			viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
			viewInfo.subresourceRange.baseMipLevel = 0;
			viewInfo.subresourceRange.levelCount = 1;
			viewInfo.subresourceRange.baseArrayLayer = 0;
			viewInfo.subresourceRange.layerCount = 1;

			if (vkCreateImageView(m_Device.GetDevice(), &viewInfo, nullptr, &m_DepthImageViews[i]) != VK_SUCCESS) {
				throw std::runtime_error("failed to create texture image view!");
			}
		}
	}

	VkSurfaceFormatKHR SwapChain::ChooseSwapSurfaceFormat(
			const std::vector<VkSurfaceFormatKHR> &availableFormats) {
		for (const auto &availableFormat : availableFormats) {
			if (availableFormat.format == VK_FORMAT_R8G8B8A8_UNORM &&
					availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
				return availableFormat;
			}
		}

		return availableFormats[0];
	}

	VkPresentModeKHR SwapChain::ChooseSwapPresentMode(
			const std::vector<VkPresentModeKHR> &availablePresentModes) {
		for (const auto &availablePresentMode : availablePresentModes) {
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
				std::cout << "Present mode: Mailbox" << std::endl;
				return availablePresentMode;
			}
		}

		// for (const auto &availablePresentMode : availablePresentModes) {
		//	 if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
		//		 std::cout << "Present mode: Immediate" << std::endl;
		//		 return availablePresentMode;
		//	 }
		// }

		std::cout << "Present mode: V-Sync" << std::endl;
		return VK_PRESENT_MODE_FIFO_KHR;
	}
	VkExtent2D SwapChain::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities) const
	{
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
			return capabilities.currentExtent;
			
		VkExtent2D actualExtent = m_WindowExtent;
		actualExtent.width = std::max(
				capabilities.minImageExtent.width,
				std::min(capabilities.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max(
				capabilities.minImageExtent.height,
				std::min(capabilities.maxImageExtent.height, actualExtent.height));

		return actualExtent;
	}

	VkFormat SwapChain::FindDepthFormat() const
	{
		return m_Device.FindSupportedFormat(
				{VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
				VK_IMAGE_TILING_OPTIMAL,
				VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
	}

}
