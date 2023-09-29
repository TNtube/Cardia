#pragma once

#include <vector>

#include <volk.h>

#include "Cardia/Math/Vector2.hpp"
#include "Cardia/Math/Vector3.hpp"
#include "Vertex.hpp"


namespace Cardia
{
	class Mesh
	{
	public:
		Mesh() = default;
		std::vector<Vertex>& GetVertices() { return  m_Vertices; }
		std::vector<uint32_t>& GetIndices() { return  m_Indices; }
		uint32_t GetMaterialIndex() const { return m_MaterialIndex; }

	private:
		friend class Model;
		uint32_t m_MaterialIndex = 0;
		std::vector<Vertex> m_Vertices;
		std::vector<uint32_t> m_Indices;
	};
}
