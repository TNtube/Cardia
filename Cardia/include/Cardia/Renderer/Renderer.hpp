#pragma once
#include "Cardia/Core/Window.hpp"
#include "Device.hpp"
#include "MeshRenderer.hpp"
#include "Pipeline.hpp"
#include "SwapChain.hpp"


namespace Cardia
{
	struct UBO
	{
		glm::mat4 viewProjection;
		glm::mat4 model;
		glm::mat4 transposedInvertedModel;
	};

	class Renderer
	{
	public:
		explicit Renderer(Window& window);
		virtual ~Renderer();
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
		Buffer m_UboBuffer;

		MeshRenderer m_MeshRenderer;
	};
}
