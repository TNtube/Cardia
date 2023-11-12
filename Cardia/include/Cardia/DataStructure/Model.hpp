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
		const std::vector<AssetHandle>& GetMaterialHandles() const { return m_MaterialHandles; }
		std::vector<AssetHandle>& GetMaterialHandles() { return m_MaterialHandles; }

		static Model FromFile(const std::filesystem::path& absolutePath);
	private:
		std::vector<AssetHandle> m_MaterialHandles {};
		std::vector<Mesh> m_Meshes {};
	};
}
