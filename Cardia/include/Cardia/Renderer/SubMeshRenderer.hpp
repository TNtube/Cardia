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

		// SubMeshRenderer(const SubMeshRenderer &) = delete;
		// SubMeshRenderer &operator=(const SubMeshRenderer &) = delete;
		void SubmitSubMesh(SubMesh& subMesh);
		void Bind(VkCommandBuffer commandBuffer) const;
		void Draw(VkCommandBuffer commandBuffer) const;
	private:
		Device& m_Device;
		VkBuffer m_VertexBuffer {};
		VkDeviceMemory m_VertexBufferMemory {};
		uint32_t m_VertexCount {};
	};
}