#pragma once

// std lib headers
#include <vector>

#include "Device.hpp"
#include "Framebuffer.hpp"
#include "RenderPass.hpp"

namespace Cardia
{
	struct FrameData;
	class SwapChain
	{
	public:
		static constexpr uint8_t MAX_FRAMES_IN_FLIGHT = 2;

		SwapChain(Device &deviceRef, VkExtent2D windowExtent);
		SwapChain(Device &deviceRef, VkExtent2D extent, std::shared_ptr<SwapChain> previous);
		virtual ~SwapChain();

		SwapChain(const SwapChain &) = delete;
		SwapChain& operator=(const SwapChain &) = delete;

		const Framebuffer& GetFrameBuffer(uint32_t index) const { return m_SwapChainFramebuffers[index]; }
		RenderPass& GetRenderPass() const { return *m_RenderPass; }
		VkImageView GetImageView(uint32_t index) const { return m_SwapChainImageViews[index]; }
		size_t ImageCount() const { return m_SwapChainImages.size(); }
		VkFormat GetSwapChainImageFormat() const { return m_SwapChainImageFormat; }
		VkExtent2D GetExtent() const { return m_SwapChainExtent; }
		uint32_t Width() const { return m_SwapChainExtent.width; }
		uint32_t Height() const { return m_SwapChainExtent.height; }

		float ExtentAspectRatio() const
		{
			return static_cast<float>(m_SwapChainExtent.width) / static_cast<float>(m_SwapChainExtent.height);
		}
		VkFormat FindDepthFormat() const;

		VkResult AcquireNextImage(const FrameData& frame, uint32_t *imageIndex) const;
		VkResult SubmitCommandBuffers(const FrameData& frame, const uint32_t *imageIndex) const;

	private:
		void Init();
		void CreateSwapChain();
		void CreateImageViews();
		void CreateDepthResources();
		void CreateRenderPass();
		void CreateFramebuffers();

		// Helper functions
		static VkSurfaceFormatKHR ChooseSwapSurfaceFormat(
				const std::vector<VkSurfaceFormatKHR> &availableFormats);
		static VkPresentModeKHR ChooseSwapPresentMode(
				const std::vector<VkPresentModeKHR> &availablePresentModes);
		VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities) const;

		VkFormat m_SwapChainImageFormat {};
		VkExtent2D m_SwapChainExtent {};

		std::vector<Framebuffer> m_SwapChainFramebuffers;
		std::unique_ptr<RenderPass> m_RenderPass {};

		std::vector<VkImage> m_DepthImages;
		std::vector<VkDeviceMemory> m_DepthImageMemories;
		std::vector<VkImageView> m_DepthImageViews;
		std::vector<VkImage> m_SwapChainImages;
		std::vector<VkImageView> m_SwapChainImageViews;

		Device &m_Device;
		VkExtent2D m_WindowExtent {};

		VkSwapchainKHR m_SwapChain {};
		std::shared_ptr<SwapChain> m_PreviousSwapChain;
	};
}
