#pragma once
#include "Cardia/Core/Window.hpp"
#include "Device.hpp"
#include "MeshRenderer.hpp"
#include "Pipeline.hpp"
#include "SwapChain.hpp"


namespace Cardia
{
	class RenderContext
	{
	public:
		explicit RenderContext(Window& window);
		virtual ~RenderContext();
		void DrawFrame();

		Device& GetDevice() { return m_Device; }

	private:
		void CreatePipelineLayout();
		void CreatePipeline();
		void CreateCommandBuffers();
		void FreeCommandBuffers();
		void RecreateSwapChain();
		void RecordCommandBuffer(uint32_t imageIndex) const;
		

	private:
		friend class ImGuiLayer;
		Window& m_Window;
		Device m_Device;
		std::unique_ptr<SwapChain> m_SwapChain;
		std::unique_ptr<Pipeline> m_Pipeline;
		VkPipelineLayout m_PipelineLayout {};
		std::vector<VkCommandBuffer> m_CommandBuffers;
		MeshRenderer m_MeshRenderer;
		
	};
}
