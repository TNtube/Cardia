#pragma once
#include <vulkan/vulkan_core.h>


namespace Cardia
{
	class Window;

	class SwapChain
	{
	public:
		explicit SwapChain(const Window& window);
		virtual ~SwapChain();

		void CreateFramebuffers();

	private:
		void CreateSwapChain();
		void CreateImageViews();
		void CreateDepthResources();
		const Window& m_Window;
		
		VkSwapchainKHR m_SwapChain {};
		std::vector<VkImage> m_Images {};
		VkFormat m_ImageFormat {};
		VkExtent2D m_Extent {};
		
		std::vector<VkImageView> m_ImageViews;
		
		VkImage m_DepthImage {};
		VkDeviceMemory m_DepthImageMemory {};
		VkImageView m_DepthImageView {};
		
		std::vector<VkFramebuffer> m_SwapChainFramebuffers;
	};
}
