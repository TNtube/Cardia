#pragma once
#include <memory>
#include <Cardia/Renderer/Material.hpp>
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
		const std::vector<MaterialInstance>& GetMaterialInstances() const { return  m_MaterialInstances; }

		static std::shared_ptr<Mesh> ReadMeshFromFile(Renderer& renderer, const std::string& path);

	private:
		std::vector<MaterialInstance> m_MaterialInstances;
		std::vector<SubMesh> m_SubMeshes {};
	};
}
