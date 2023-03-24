#pragma once


#include "Cardia/DataStructure/SubMesh.hpp"
#include "VertexArray.hpp"
#include "Buffer.hpp"

namespace Cardia
{
	class SubMeshRenderer
	{
	public:
		SubMeshRenderer() = default;

		void SubmitSubMesh(SubMesh& subMesh);
		void Draw();
	private:
		std::unique_ptr<VertexArray> m_VertexArray;
		std::unique_ptr<VertexBuffer> m_VertexBuffer;
		std::unique_ptr<IndexBuffer> m_IndexBuffer;
	};
}