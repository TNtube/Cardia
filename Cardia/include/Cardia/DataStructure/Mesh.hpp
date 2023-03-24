#pragma once
#include <glm/glm.hpp>
#include <memory>
#include "SubMesh.hpp"
#include "Cardia/Renderer/Texture.hpp"

namespace Cardia
{
	class Mesh
	{
	public:
		Mesh() = default;
		std::vector<SubMesh>& GetSubMeshes() { return  m_SubMeshes; }
		const std::vector<SubMesh>& GetSubMeshes() const { return  m_SubMeshes; }
		std::vector<std::shared_ptr<Texture2D>>& GetMaterials() { return  m_Materials; }
		const std::vector<std::shared_ptr<Texture2D>>& GetMaterials() const { return  m_Materials; }

		static Mesh ReadMeshFromFile(const std::string& path);

	private:
		std::vector<std::shared_ptr<Texture2D>> m_Materials;
		std::vector<SubMesh> m_SubMeshes {};

	};
}
