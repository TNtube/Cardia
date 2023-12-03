#include "cdpch.hpp"

#include "Cardia/Assets/Importers/Importer.hpp"

#include "Cardia/Application.hpp"
#include "Cardia/Assets/Importers/TextureImporter.hpp"
#include "Cardia/Assets/AssetsManager.hpp"


namespace Cardia
{
	std::shared_ptr<Importer> Importer::LoadFromPath(const std::filesystem::path& path)
	{
		auto withoutExt = std::filesystem::absolute(path);
		withoutExt.replace_extension();
		const auto extension = withoutExt.extension().string();

		switch (ExtensionToAssetType(extension))
		{
			case AssetType::Texture:
			{
				auto importer = Serializer<TextureImporter>::Deserialize(path);
				return std::make_shared<TextureImporter>(std::move(importer));
			}
			default:
				auto importer = Serializer<DefaultImporter>::Deserialize(path);
				return std::make_shared<DefaultImporter>(std::move(importer));
		}

	}

	std::shared_ptr<Importer> Importer::Instantiate(const std::filesystem::path& extension)
	{
		switch (ExtensionToAssetType(extension.string())) {
			case AssetType::Texture:
				return std::make_shared<TextureImporter>();
			default:
				return std::make_shared<DefaultImporter>();
		}
	}

	void DefaultImporter::Serialize(const std::filesystem::path& path)
	{
		Serializer serializer(*this);
		serializer.Serialize(path);
	}
}
