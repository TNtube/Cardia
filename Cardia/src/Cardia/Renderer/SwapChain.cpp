#include "cdpch.hpp"
#include "Cardia/Renderer/SwapChain.hpp"

#include <GLFW/glfw3.h>

#include "Cardia/Renderer/RenderContext.hpp"
#include "Cardia/Core/Window.hpp"


namespace Cardia
{
	VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
		for (const auto& availableFormat : availableFormats) {
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB
			&& availableFormat.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR) {
				return availableFormat;
			}
		}

		return availableFormats[0];
	}

	VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
		for (const auto& availablePresentMode : availablePresentModes) {
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
				return availablePresentMode;
			}
		}
		return VK_PRESENT_MODE_FIFO_KHR;
	}

	VkExtent2D ChooseSwapExtent(GLFWwindow* window, const VkSurfaceCapabilitiesKHR& capabilities) {
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
			return capabilities.currentExtent;
		}

		int width, height;
		glfwGetFramebufferSize(window, &width, &height);

		VkExtent2D actualExtent = {
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		};

		actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

		return actualExtent;
	}

	SwapChain::SwapChain(const Window& window): m_Window(window)
	{
		CreateSwapChain();
		CreateImageViews();
	}

	SwapChain::~SwapChain()
	{
		const auto& device = RenderContext::Instance().GetDevice();
		
		for (const auto imageView : m_ImageViews) {
			vkDestroyImageView(device, imageView, nullptr);
		}
		vkDestroySwapchainKHR(device, m_SwapChain, nullptr);
	}

	void SwapChain::CreateFramebuffers()
	{
		CreateDepthResources();
		m_SwapChainFramebuffers.resize(m_ImageViews.size());

		for (std::size_t i = 0; i < m_ImageViews.size(); ++i)
		{
			std::array<VkImageView, 2> attachments = {
				m_ImageViews[i],
				m_DepthImageView
			};

			VkFramebufferCreateInfo framebufferInfo {};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = m_RenderPass;
			framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
			framebufferInfo.pAttachments = attachments.data();
			framebufferInfo.width = m_Extent.width;
			framebufferInfo.height = m_Extent.height;
			framebufferInfo.layers = 1;

			if (vkCreateFramebuffer(m_Device, &framebufferInfo, nullptr, &m_SwapChainFramebuffers[i]) != VK_SUCCESS)
				throw std::runtime_error("Vulkan : failed to create frameBuffers !");
		}
	}

	void SwapChain::CreateSwapChain()
	{
		const RenderContext::SwapChainSupportDetails swapChainSupport(RenderContext::Instance().GetPhysicalDevice());
		const auto surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
		const auto presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
		const auto extent = ChooseSwapExtent(static_cast<GLFWwindow*>(m_Window.getNativeWin()), swapChainSupport.capabilities);

		uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

		if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
			imageCount = swapChainSupport.capabilities.maxImageCount;
		}

		VkSwapchainCreateInfoKHR createInfo {};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = RenderContext::Instance().GetSurface();

		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		const RenderContext::QueueFamilyIndices indices(RenderContext::Instance().GetPhysicalDevice());
		const uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

		if (indices.graphicsFamily != indices.presentFamily) {
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		} else {
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0;
			createInfo.pQueueFamilyIndices = nullptr;
		}

		createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;

		createInfo.oldSwapchain = VK_NULL_HANDLE;

		const auto& device = RenderContext::Instance().GetDevice();

		if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &m_SwapChain) != VK_SUCCESS) {
			throw std::runtime_error("failed to create swap chain!");
		}

		vkGetSwapchainImagesKHR(device, m_SwapChain, &imageCount, nullptr);
		m_Images.resize(imageCount);
		vkGetSwapchainImagesKHR(device, m_SwapChain, &imageCount, m_Images.data());
		m_ImageFormat = surfaceFormat.format;
		m_Extent = extent;
	}

	void SwapChain::CreateImageViews()
	{
		m_ImageViews.resize(m_Images.size());

		for (uint32_t i = 0; i < m_Images.size(); i++) {
			m_ImageViews[i] = RenderContext::Instance().CreateImageView(m_Images[i], m_ImageFormat, VK_IMAGE_ASPECT_COLOR_BIT);
		}
	}
	
	void SwapChain::CreateDepthResources() {
		const VkFormat depthFormat = RenderContext::Instance().FindDepthFormat();
		RenderContext::Instance().CreateImage(m_Extent.width, m_Extent.height, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_DepthImage, m_DepthImageMemory);
		m_DepthImageView = RenderContext::Instance().CreateImageView(m_DepthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);

		RenderContext::Instance().TransitionImageLayout(m_DepthImage, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
	}
}
