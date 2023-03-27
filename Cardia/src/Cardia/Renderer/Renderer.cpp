#include "cdpch.hpp"
#include "Cardia/Renderer/OpenGL/OpenGLShader.hpp"
#include "Cardia/Renderer/Renderer.hpp"

namespace Cardia
{
	std::unique_ptr<Renderer::SceneData> Renderer::s_SceneData = std::make_unique<Renderer::SceneData>();

	SingleTimeCommand::SingleTimeCommand()
	{
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = Renderer::Instance().GetCommandPool();
		allocInfo.commandBufferCount = 1;
		
		vkAllocateCommandBuffers(Renderer::Instance().GetDevice(), &allocInfo, &m_CommandBuffer);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(m_CommandBuffer, &beginInfo);
	}

	SingleTimeCommand::~SingleTimeCommand()
	{
		const Renderer& renderer = Renderer::Instance();
		vkEndCommandBuffer(m_CommandBuffer);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &m_CommandBuffer;

		vkQueueSubmit(renderer.GetGraphicQueue(), 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(renderer.GetGraphicQueue());

		vkFreeCommandBuffers(renderer.GetDevice(), renderer.GetCommandPool(), 1, &m_CommandBuffer);
	}

	uint32_t Renderer::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const
	{
		VkPhysicalDeviceMemoryProperties memoryProperties {};
		vkGetPhysicalDeviceMemoryProperties(m_PhysicalDevice, &memoryProperties);

		for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++) {
			if (typeFilter & (1 << i) && (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties) {
				return i;
			}
		}
		throw std::runtime_error("Vulkan : failed to find a suitable memory type !");
	}

	void Renderer::beginScene(Camera& camera)
	{
		s_SceneData->ViewProjectionMatrix = camera.getViewProjectionMatrix();
	}

	void Renderer::endScene()
	{
	}

	void Renderer::submit(const VertexArray* vertexArray, Shader* shader, const glm::mat4& transform)
	{
		shader->bind();
		shader->setMat4("u_ViewProjection", s_SceneData->ViewProjectionMatrix);
		shader->setMat4("u_Model", transform);
		shader->setMat3("u_TransposedInvertedModel", glm::transpose(glm::inverse(glm::mat3(transform))));

		vertexArray->bind();
		RenderAPI::get().drawIndexed(vertexArray);
	}
}