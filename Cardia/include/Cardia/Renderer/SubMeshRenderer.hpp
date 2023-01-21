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
		std::unique_ptr<VertexArray> m_vertexArray;
		std::unique_ptr<VertexBuffer> m_vertexBuffer;
		std::unique_ptr<IndexBuffer> m_indexBuffer;
	};
}