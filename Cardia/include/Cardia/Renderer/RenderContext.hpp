#pragma once
#include "Cardia/Core/Window.hpp"
#include "Device.hpp"
#include "Pipeline.hpp"
#include "SwapChain.hpp"


namespace Cardia
{
	class RenderContext
	{
	public:
		explicit RenderContext(Window& window);
		virtual ~RenderContext() = default;
		void Init();
		void DrawFrame();

	private:
		void CreatePipelineLayout();
		void CreatePipeline();
		void CreateCommandBuffers();
		

	private:
		Window& m_Window;
		Device m_Device;
		SwapChain m_SwapChain;
		std::unique_ptr<Pipeline> m_Pipeline;
		VkPipelineLayout m_PipelineLayout {};
		std::vector<VkCommandBuffer> m_CommandBuffers;
		
	};
}
