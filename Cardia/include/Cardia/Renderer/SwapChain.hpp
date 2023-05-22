#pragma once

// std lib headers
#include <vector>

#include "Device.hpp"
#include "Framebuffer.hpp"

namespace Cardia
{
	class SwapChain
	{
	public:
		static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

		SwapChain(Device &deviceRef, VkExtent2D windowExtent);
		SwapChain(Device &deviceRef, VkExtent2D extent, std::shared_ptr<SwapChain> previous);
		virtual ~SwapChain();

		SwapChain(const SwapChain &) = delete;
		SwapChain& operator=(const SwapChain &) = delete;

		VkFramebuffer GetFrameBuffer(int index) const { return m_SwapChainFramebuffers[index].GetFramebuffer(); }
		VkRenderPass GetRenderPass() const { return m_RenderPass; }
		VkImageView GetImageView(int index) const { return m_SwapChainImageViews[index]; }
		size_t ImageCount() const { return m_SwapChainImages.size(); }
		VkFormat GetSwapChainImageFormat() const { return m_SwapChainImageFormat; }
		VkExtent2D GetSwapChainExtent() const { return m_SwapChainExtent; }
		uint32_t Width() const { return m_SwapChainExtent.width; }
		uint32_t Height() const { return m_SwapChainExtent.height; }

		float ExtentAspectRatio() const
		{
			return static_cast<float>(m_SwapChainExtent.width) / static_cast<float>(m_SwapChainExtent.height);
		}
		VkFormat FindDepthFormat() const;

		VkResult AcquireNextImage(uint32_t *imageIndex);
		VkResult SubmitCommandBuffers(const VkCommandBuffer *buffers, uint32_t *imageIndex);

	 private:
		void Init();
		void CreateSwapChain();
		void CreateImageViews();
		void CreateDepthResources();
		void CreateRenderPass();
		void CreateFramebuffers();
		void CreateSyncObjects();

		// Helper functions
		VkSurfaceFormatKHR ChooseSwapSurfaceFormat(
				const std::vector<VkSurfaceFormatKHR> &availableFormats);
		VkPresentModeKHR ChooseSwapPresentMode(
				const std::vector<VkPresentModeKHR> &availablePresentModes);
		VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities) const;

		VkFormat m_SwapChainImageFormat {};
		VkExtent2D m_SwapChainExtent {};

		std::vector<Framebuffer> m_SwapChainFramebuffers;
		VkRenderPass m_RenderPass {};

		std::vector<VkImage> m_DepthImages;
		std::vector<VkDeviceMemory> m_DepthImageMemories;
		std::vector<VkImageView> m_DepthImageViews;
		std::vector<VkImage> m_SwapChainImages;
		std::vector<VkImageView> m_SwapChainImageViews;

		Device &m_Device;
		VkExtent2D m_WindowExtent {};

		VkSwapchainKHR swapChain {};
		std::shared_ptr<SwapChain> m_PreviousSwapChain;

		std::vector<VkSemaphore> imageAvailableSemaphores;
		std::vector<VkSemaphore> renderFinishedSemaphores;
		std::vector<VkFence> inFlightFences;
		std::vector<VkFence> imagesInFlight;
		size_t currentFrame = 0;
	};
}
