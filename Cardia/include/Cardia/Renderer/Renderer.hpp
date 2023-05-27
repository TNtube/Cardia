#pragma once
#include "Cardia/Core/Window.hpp"
#include "Device.hpp"
#include "MeshRenderer.hpp"
#include "Pipeline.hpp"
#include "SwapChain.hpp"


namespace Cardia
{
	class Renderer
	{
	public:
		explicit Renderer(Window& window);
		virtual ~Renderer();

		VkCommandBuffer Begin();
		void End();
		void BeginSwapChainRenderPass() const;
		void EndSwapChainRenderPass() const;

		Device& GetDevice() { return m_Device; }
		SwapChain& GetSwapChain() { return *m_SwapChain; }
		uint32_t GetFrameIndex() { return m_CurrentImageIndex; }

	private:
		void CreateCommandBuffers();
		void FreeCommandBuffers();
		void RecreateSwapChain();
		void RecordCommandBuffer(uint32_t imageIndex) const;
		

	private:
		friend class ImGuiLayer;
		Window& m_Window;
		Device m_Device;
		std::unique_ptr<SwapChain> m_SwapChain;
		std::vector<VkCommandBuffer> m_CommandBuffers;

		uint32_t m_CurrentImageIndex {};
		uint32_t m_CurrentFrameIndex {};

		MeshRenderer m_MeshRenderer;
	};
}
