#pragma once


#include "Cardia/DataStructure/SubMesh.hpp"
#include "VertexArray.hpp"
#include "Buffer.hpp"

namespace Cardia
{
	class SubMeshRenderer
	{
	public:
		SubMeshRenderer(Device &device, SubMesh &subMesh);
		~SubMeshRenderer();
		SubMeshRenderer(SubMeshRenderer && other) noexcept;

		SubMeshRenderer(const SubMeshRenderer &) = delete;
		SubMeshRenderer &operator=(const SubMeshRenderer &) = delete;
		void SubmitSubMesh(SubMesh& subMesh);
		void Bind(VkCommandBuffer commandBuffer) const;
		void Draw(VkCommandBuffer commandBuffer) const;
	private:
		Device& m_Device;
		uint32_t m_VertexCount;
		uint32_t m_IndexCount;
		Buffer m_VertexBuffer;
		Buffer m_IndexBuffer;
	};
}