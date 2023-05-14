#include <Cardia/Renderer/RenderAPI.hpp>
#include <Cardia/Renderer/Renderer2D.hpp>
#include "cdpch.hpp"

#include "Cardia/Renderer/SubMeshRenderer.hpp"

namespace Cardia
{
	SubMeshRenderer::SubMeshRenderer(Device& device, SubMesh &subMesh) : m_Device(device)
	{
		SubmitSubMesh(subMesh);
	}

	SubMeshRenderer::~SubMeshRenderer()
	{
		vkDestroyBuffer(m_Device.GetDevice(), m_VertexBuffer, nullptr);
		vkFreeMemory(m_Device.GetDevice(), m_VertexBufferMemory, nullptr);
	}

	void SubMeshRenderer::SubmitSubMesh(SubMesh &subMesh)
	{
		m_VertexCount = static_cast<uint32_t>(subMesh.GetVertices().size());
		assert(m_VertexCount >= 3 && "Vertex count must be at least 3");
		const VkDeviceSize bufferSize = sizeof(subMesh.GetVertices()[0]) * m_VertexCount;
		m_Device.CreateBuffer(
		    bufferSize,
		    VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		    m_VertexBuffer,
		    m_VertexBufferMemory);

		void *data;
		vkMapMemory(m_Device.GetDevice(), m_VertexBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, subMesh.GetVertices().data(), bufferSize);
		vkUnmapMemory(m_Device.GetDevice(), m_VertexBufferMemory);
	}

	void SubMeshRenderer::Bind(VkCommandBuffer commandBuffer) const
	{
		const VkBuffer buffers[] = {m_VertexBuffer};
		constexpr VkDeviceSize offsets[] = {0};
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
	}

	void SubMeshRenderer::Draw(VkCommandBuffer commandBuffer) const
	{
		vkCmdDraw(commandBuffer, m_VertexCount, 1, 0, 0);
	}
}