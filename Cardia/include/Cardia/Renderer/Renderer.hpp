#pragma once
#include "Descriptors.hpp"
#include "Cardia/Core/Window.hpp"
#include "Device.hpp"
#include "MeshRenderer.hpp"
#include "Pipeline.hpp"
#include "SwapChain.hpp"


namespace Cardia
{
	struct UboData
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

		VkCommandBuffer Begin();
		void End();
		void BeginSwapChainRenderPass() const;
		void EndSwapChainRenderPass() const;

		Device& GetDevice() { return m_Device; }
		SwapChain& GetSwapChain() const { return *m_SwapChain; }
		DescriptorPool& GetDescriptorSetPool() const { return *m_DescriptorPool; }
		Pipeline& GetPipeline() const { return *m_Pipeline; }
		VkPipelineLayout GetPipelineLayout() const { return m_PipelineLayout; }
		DescriptorSetLayout& GetDescriptorSetLayout() const { return *m_DescriptorSetLayout; }
		VkDescriptorSet& GetCurrentDescriptorSet() { return m_DescriptorSets[m_CurrentFrameIndex]; }
		Buffer& GetCurrentUboBuffer() const { return *m_UboBuffers[m_CurrentFrameIndex]; }
		uint32_t GetFrameIndex() const { return m_CurrentImageIndex; }

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

		std::unique_ptr<DescriptorPool> m_DescriptorPool;

		std::vector<std::unique_ptr<Buffer>> m_UboBuffers;
		std::vector<VkDescriptorSet> m_DescriptorSets{SwapChain::MAX_FRAMES_IN_FLIGHT};
		std::unique_ptr<DescriptorSetLayout> m_DescriptorSetLayout;
		VkPipelineLayout m_PipelineLayout {}; // TODO: Remove ?
		std::shared_ptr<Pipeline> m_Pipeline {};

		MeshRenderer m_MeshRenderer;
	};
}
