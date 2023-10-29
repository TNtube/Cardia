#pragma once
#include "Cardia/Core/Window.hpp"
#include "Descriptors.hpp"
#include "Device.hpp"
#include "MeshRenderer.hpp"
#include "Pipeline.hpp"
#include "RenderPass.hpp"
#include "SwapChain.hpp"
#include "Cardia/Math/Matrix4.hpp"
#include "Skybox.hpp"


namespace Cardia
{
	struct UboData
	{
		Matrix4f ViewProjection;
		Vector4f CameraPositionAndTime;
	};
	
	struct PushConstantData
	{
		Matrix4f Model;
		Matrix4f TransposedInvertedModel;
	};

	struct FrameData final
	{
		VkSemaphore PresentSemaphore {};
		VkSemaphore RenderSemaphore {};
		VkFence RenderFence {};

		VkCommandBuffer MainCommandBuffer {};

		std::shared_ptr<Buffer> MainUboBuffer;
		std::shared_ptr<DescriptorSet> MainUboDescriptorSet;

		std::shared_ptr<Buffer> SkyboxUboBuffer;
		std::shared_ptr<DescriptorSet> SkyboxUboDescriptorSet;
	};

	struct SkyboxUboData
	{
		Matrix4f Projection;
		Matrix4f Model;
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

		void WaitIdle() const;

		const Device& GetDevice() const { return m_Device; }
		SwapChain& GetSwapChain() const { return *m_SwapChain; }
		DescriptorAllocator& GetDescriptorAllocator() const { return *m_DescriptorAllocator; }
		DescriptorLayoutCache& GetDescriptorLayoutCache() const { return *m_DescriptorLayoutCache; }
		Pipeline& GetMainPipeline() const { return *m_MainPipeline; }
		const FrameData& GetCurrentFrame() const { return m_Frames[m_CurrentFrameNumber % SwapChain::MAX_FRAMES_IN_FLIGHT]; }
		uint32_t GetCurrentImageIndex() const { return m_CurrentImageIndex; }
		std::shared_ptr<Texture> GetWhiteTexture() const { return m_WhiteTexture; }
		std::shared_ptr<Texture> GetNormalTexture() const { return m_NormalTexture; }

		DescriptorSetLayout& GetMaterialDescriptorSetLayout() const { return *m_MaterialDescriptorSetLayout; }
		const Skybox& GetSkybox();


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
		std::shared_ptr<Texture> m_WhiteTexture;
		std::shared_ptr<Texture> m_NormalTexture;

		std::shared_ptr<DescriptorSetLayout> m_MaterialDescriptorSetLayout;

		uint32_t m_CurrentImageIndex {};
		uint32_t m_CurrentFrameNumber {};

		std::vector<FrameData> m_Frames{SwapChain::MAX_FRAMES_IN_FLIGHT};

		std::shared_ptr<Pipeline> m_MainPipeline {};

		std::shared_ptr<Skybox> m_Skybox;
	};
}
