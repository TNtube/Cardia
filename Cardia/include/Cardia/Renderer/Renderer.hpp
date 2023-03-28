#pragma once

#include <vulkan/vulkan_core.h>

#include "RenderAPI.hpp"
#include "Camera.hpp"
#include "Shader.hpp"


namespace Cardia
{
	class Window;

	class SingleTimeCommand
	{
	public:
		SingleTimeCommand();
		SingleTimeCommand(const SingleTimeCommand&) = delete;
		SingleTimeCommand(SingleTimeCommand&&) = delete;
		SingleTimeCommand& operator=(const SingleTimeCommand&) = delete;
		SingleTimeCommand& operator=(SingleTimeCommand&&) = delete;
		const VkCommandBuffer& GetCommandBuffer() const { return m_CommandBuffer; }

		virtual ~SingleTimeCommand();

	private:
		VkCommandBuffer m_CommandBuffer {};
		
	};

	class Renderer
	{
	public:
		static Renderer& Instance() { static Renderer m_Renderer; return m_Renderer; }

		void Init(Window& window);
		void Finalize();

		const VkInstance& GetVkInstance() const { return m_VkInstance; }
		const VkDevice& GetDevice() const { return m_Device; }
		const VkCommandPool& GetCommandPool() const { return m_CommandPool; }
		const VkQueue& GetGraphicQueue() const { return m_GraphicsQueue; }
		const VkPhysicalDevice& GetPhysicalDevice() const { return m_PhysicalDevice; }
		uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;
		
		static void beginScene(Camera& camera);
		static void endScene();
		static void submit(const VertexArray* vertexArray,
				   Shader* shader, const glm::mat4& transform = glm::mat4(1.0f));
		inline static RenderAPI::API& getAPI() { return RenderAPI::getAPI(); }

		struct QueueFamilyIndices {
			explicit QueueFamilyIndices(VkPhysicalDevice device);
			std::optional<uint32_t> graphicsFamily;
			std::optional<uint32_t> presentFamily;

			bool IsComplete() const
			{
				return graphicsFamily.has_value() && presentFamily.has_value();
			}
		};

	private:
		struct SwapChainSupportDetails {
			explicit SwapChainSupportDetails(VkPhysicalDevice device);
			VkSurfaceCapabilitiesKHR capabilities {};
			std::vector<VkSurfaceFormatKHR> formats;
			std::vector<VkPresentModeKHR> presentModes;
		};
		
		Renderer() = default;
		void CreateVkInstance();
		void SetupDebugMessage();
		void PickPhysicalDevice();
		bool IsDeviceSuitable(VkPhysicalDevice device) const;

		void CreateLogicalDevice();

		
		struct SceneData
		{
			glm::mat4 ViewProjectionMatrix;
		};

		static std::unique_ptr<SceneData> s_SceneData;

		VkInstance m_VkInstance {};
		VkDebugUtilsMessengerEXT m_DebugMessenger {};
		
		VkSurfaceKHR m_Surface {};
		
		VkPhysicalDevice m_PhysicalDevice {};
		VkDevice m_Device {};

		VkQueue m_GraphicsQueue {};
		VkQueue m_PresentQueue {};
		
		VkCommandPool m_CommandPool {};
	};
}