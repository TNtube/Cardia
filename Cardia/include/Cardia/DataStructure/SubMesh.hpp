#pragma once

#include <vector>
#include "Vertex.hpp"


namespace Cardia
{
	class SubMesh
	{
	public:
		SubMesh() = default;
		std::vector<Vertex>& GetVertices() { return  m_Vertices; }
		const std::vector<Vertex>& GetVertices() const { return  m_Vertices; }
		std::vector<uint32_t>& GetIndices() { return  m_Indices; }
		const std::vector<uint32_t>& GetIndices() const { return  m_Indices; }

	private:
		std::vector<Vertex> m_Vertices;
		std::vector<uint32_t> m_Indices;
	};
}