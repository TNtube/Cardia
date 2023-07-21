#pragma once
#include "Cardia/Core/Window.hpp"
#include "Descriptors.hpp"
#include "Device.hpp"
#include "MeshRenderer.hpp"
#include "Pipeline.hpp"
#include "RenderPass.hpp"
#include "SwapChain.hpp"


namespace Cardia
{
	struct UboData
	{
		glm::mat4 ViewProjection;
	};
	
	struct PushConstantData
	{
		glm::mat4 Model;
		glm::mat4 TransposedInvertedModel;
	};

	struct FrameData final
	{
		VkSemaphore PresentSemaphore {};
		VkSemaphore RenderSemaphore {};
		VkFence RenderFence {};

		VkCommandBuffer MainCommandBuffer {};

		std::shared_ptr<Buffer> UboBuffer;
		std::shared_ptr<DescriptorSet> UboDescriptorSet;
	};

	class Renderer
	{
	public:
		Renderer(const Renderer& other) = delete;
		Renderer(Renderer&& other) noexcept = delete;
		Renderer& operator=(const Renderer& other) = delete;
		Renderer& operator=(Renderer&& other) noexcept = delete;

		explicit Renderer(Window& window);
		virtual ~Renderer();

		VkCommandBuffer Begin();
		bool End();
		void BeginRenderPass(const Framebuffer& frameBuffer, const RenderPass& renderPass) const;
		void BeginSwapChainRenderPass() const;
		void EndRenderPass() const;

		Device& GetDevice() { return m_Device; }
		SwapChain& GetSwapChain() const { return *m_SwapChain; }
		DescriptorAllocator& GetDescriptorAllocator() const { return *m_DescriptorAllocator; }
		DescriptorLayoutCache& GetDescriptorLayoutCache() const { return *m_DescriptorLayoutCache; }
		Pipeline& GetPipeline() const { return *m_Pipeline; }
		PipelineLayout& GetPipelineLayout() const { return *m_PipelineLayout; }
		const FrameData& GetCurrentFrame() const { return m_Frames[m_CurrentFrameNumber % SwapChain::MAX_FRAMES_IN_FLIGHT]; }
		uint32_t GetCurrentImageIndex() const { return m_CurrentImageIndex; }
		Texture2D& GetWhiteTexture() const { return *m_WhiteTexture; }

	private:
		void CreateCommandBuffers();
		void CreateSyncObjects();
		void RecreateSwapChain();
		

	private:
		Window& m_Window;
		Device m_Device;
		std::unique_ptr<SwapChain> m_SwapChain;
		std::unique_ptr<DescriptorAllocator> m_DescriptorAllocator;
		std::unique_ptr<DescriptorLayoutCache> m_DescriptorLayoutCache;
		std::unique_ptr<Texture2D> m_WhiteTexture;

		uint32_t m_CurrentImageIndex {};
		uint32_t m_CurrentFrameNumber {};

		std::vector<FrameData> m_Frames{SwapChain::MAX_FRAMES_IN_FLIGHT};

		std::unique_ptr<PipelineLayout> m_PipelineLayout; // TODO: Remove ?
		std::shared_ptr<Pipeline> m_Pipeline {};

		MeshRenderer m_MeshRenderer;
	};
}
