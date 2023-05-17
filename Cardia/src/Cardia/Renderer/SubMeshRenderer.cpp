#include <Cardia/Renderer/RenderAPI.hpp>
#include <Cardia/Renderer/Renderer2D.hpp>
#include "cdpch.hpp"

#include "Cardia/Renderer/SubMeshRenderer.hpp"

namespace Cardia
{
	SubMeshRenderer::SubMeshRenderer(Device& device, SubMesh &subMesh)
		: m_Device(device),
		m_VertexCount(static_cast<uint32_t>(subMesh.GetVertices().size())),
		m_VertexBuffer(device,
			sizeof(subMesh.GetVertices()[0]) * m_VertexCount,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
	{
		SubmitSubMesh(subMesh);
	}

	SubMeshRenderer::~SubMeshRenderer() = default;

	SubMeshRenderer::SubMeshRenderer(SubMeshRenderer&& other) noexcept : m_Device(other.m_Device), m_VertexCount(other.m_VertexCount), m_VertexBuffer(std::move(other.m_VertexBuffer))
	{
	}

	void SubMeshRenderer::SubmitSubMesh(SubMesh &subMesh)
	{
		m_VertexCount = static_cast<uint32_t>(subMesh.GetVertices().size());
		assert(m_VertexCount >= 3 && "Vertex count must be at least 3");
		const VkDeviceSize bufferSize = sizeof(subMesh.GetVertices()[0]) * m_VertexCount;
		m_VertexBuffer.UploadData(bufferSize, subMesh.GetVertices().data());
	}

	void SubMeshRenderer::Bind(VkCommandBuffer commandBuffer) const
	{
		const VkBuffer buffers[] = {m_VertexBuffer.GetBuffer()};
		constexpr VkDeviceSize offsets[] = {0};
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
	}

	void SubMeshRenderer::Draw(VkCommandBuffer commandBuffer) const
	{
		vkCmdDraw(commandBuffer, m_VertexCount, 1, 0, 0);
	}
}