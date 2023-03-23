#pragma once
#include <glm/glm.hpp>
#include "SubMesh.hpp"

namespace Cardia
{
	class Mesh
	{
	public:
		Mesh() = default;
		std::vector<SubMesh>& GetSubMeshes() { return  m_SubMeshes; }
		const std::vector<SubMesh>& GetSubMeshes() const { return  m_SubMeshes; }

		static Mesh ReadMeshFromFile(const std::string& path);

	private:
		std::vector<SubMesh> m_SubMeshes {};

	};
}
