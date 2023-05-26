#include <Cardia/Renderer/Renderer2D.hpp>
#include "cdpch.hpp"

#include "Cardia/Renderer/SubMeshRenderer.hpp"

namespace Cardia
{
	SubMeshRenderer::SubMeshRenderer(Device& device, SubMesh &subMesh)
		: m_Device(device),
		m_VertexCount(static_cast<uint32_t>(subMesh.GetVertices().size())),
		m_IndexCount(static_cast<uint32_t>(subMesh.GetIndices().size())),
		m_VertexBuffer(device,
			sizeof(subMesh.GetVertices()[0]) * m_VertexCount,
			1,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT),
		m_IndexBuffer(device,
			sizeof(subMesh.GetIndices()[0]) * m_IndexCount,
			1,
			VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
	{
		SubmitSubMesh(subMesh);
	}

	SubMeshRenderer::~SubMeshRenderer() = default;

	SubMeshRenderer::SubMeshRenderer(SubMeshRenderer&& other) noexcept :
		m_Device(other.m_Device),
		m_VertexCount(other.m_VertexCount),
		m_IndexCount(other.m_IndexCount),
		m_VertexBuffer(std::move(other.m_VertexBuffer)),
		m_IndexBuffer(std::move(other.m_IndexBuffer))
	{
	}

	void SubMeshRenderer::SubmitSubMesh(SubMesh &subMesh)
	{
		VkDeviceSize bufferSize = sizeof(subMesh.GetVertices()[0]) * m_VertexCount;
		m_VertexBuffer.UploadData(bufferSize, subMesh.GetVertices().data());

		bufferSize = sizeof(subMesh.GetIndices()[0]) * m_IndexCount;

		Buffer stagingBuffer(
			m_Device,
			bufferSize,
			1,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		stagingBuffer.UploadData(bufferSize, subMesh.GetIndices().data());

		m_Device.CopyBuffer(stagingBuffer.GetBuffer(), m_IndexBuffer.GetBuffer(), bufferSize);
	}

	void SubMeshRenderer::Bind(VkCommandBuffer commandBuffer) const
	{
		const VkBuffer buffers[] = {m_VertexBuffer.GetBuffer()};
		constexpr VkDeviceSize offsets[] = {0};
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
		vkCmdBindIndexBuffer(commandBuffer, m_IndexBuffer.GetBuffer(), 0, VK_INDEX_TYPE_UINT32);
	}

	void SubMeshRenderer::Draw(VkCommandBuffer commandBuffer) const
	{
		vkCmdDrawIndexed(commandBuffer, m_IndexCount, 1, 0, 0, 0);
		// vkCmdDraw(commandBuffer, m_VertexCount, 1, 0, 0);
	}
}