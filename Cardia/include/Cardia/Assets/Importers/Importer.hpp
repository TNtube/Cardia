#pragma once
#include "Cardia/Renderer/Material.hpp"
#include "Cardia/Renderer/Texture.hpp"


namespace Cardia
{
	class TextureImporter;
	enum class AssetType
	{
		Invalid,
		Texture,
		Model,
		Material,
		Script,
		Scene
	};

	inline AssetType ExtensionToAssetType(const std::string& extension)
	{
		if (extension == "png" || extension == "jpg" || extension == "jpeg" || extension == "tga")
			return AssetType::Texture;

		if (extension == "obj" || extension == "fbx" || extension == "gltf")
			return AssetType::Model;

		if (extension == "mat")
			return AssetType::Material;

		if (extension == "py")
			return AssetType::Script;

		if (extension == "cardia")
			return AssetType::Scene;

		return AssetType::Invalid;
	}


	class Importer
	{
	public:
		virtual ~Importer() = default;
		virtual AssetType GetType() = 0;
		virtual void Import(std::shared_ptr<void>& asset) = 0;
		bool IsDirty() const { return m_Dirty; }
		void SetDirty() { m_Dirty = true; }

		template <typename T>
		bool IsTypeValid()
		{
			switch (GetType())
			{
				case AssetType::Texture:
					return std::is_same_v<T, Texture>;
				case AssetType::Material:
					return std::is_same_v<T, Material>;
				default:
					return false;
			}
		}

	protected:
		UUID m_Uuid = UUID::Default();
		bool m_Dirty = false;
	};
}
