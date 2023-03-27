#pragma once

#include <vulkan/vulkan_core.h>

#include "RenderAPI.hpp"
#include "Camera.hpp"
#include "Shader.hpp"


namespace Cardia
{
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

		const VkDevice& GetDevice() const { return m_Device; }
		const VkCommandPool& GetCommandPool() const { return m_CommandPool; }
		const VkQueue& GetGraphicQueue() const { return m_GraphicsQueue; }
		uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;
		
		static void beginScene(Camera& camera);
		static void endScene();
		static void submit(const VertexArray* vertexArray,
				   Shader* shader, const glm::mat4& transform = glm::mat4(1.0f));
		inline static RenderAPI::API& getAPI() { return RenderAPI::getAPI(); }

	private:
		struct SceneData
		{
			glm::mat4 ViewProjectionMatrix;
		};

		static std::unique_ptr<SceneData> s_SceneData;
		VkInstance m_VkInstance {};
		VkPhysicalDevice m_PhysicalDevice {};
		VkDevice m_Device {};
		VkQueue m_GraphicsQueue {};
		VkCommandPool m_CommandPool {};
	};
}