#pragma once
#include <memory>
#include <Cardia/Renderer/Material.hpp>
#include "Mesh.hpp"
#include "Cardia/Renderer/Texture.hpp"

namespace Cardia
{
	class Model
	{
	public:
		Model() = default;
		std::vector<Mesh>& GetMeshes() { return  m_Meshes; }
		const std::vector<Mesh>& GetMeshes() const { return  m_Meshes; }
		const std::vector<UUID>& GetMaterialHandles() const { return m_MaterialUuids; }
		std::vector<UUID>& GetMaterialHandles() { return m_MaterialUuids; }

		static Model FromFile(const std::filesystem::path& absolutePath);
	private:
		std::vector<UUID> m_MaterialUuids {};
		std::vector<Mesh> m_Meshes {};
	};
}
