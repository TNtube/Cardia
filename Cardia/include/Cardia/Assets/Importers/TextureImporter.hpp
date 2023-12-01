#pragma once
#include "Importer.hpp"
#include "Cardia/Renderer/Texture.hpp"


namespace Cardia
{
	class AssetsManager;
	class TextureImporter : public Importer
	{
	public:
		explicit TextureImporter(UUID id)
		{
			m_Uuid = std::move(id);
		}
		void Import(std::shared_ptr<void>& asset) override;
		AssetType GetType() override;

	private:
		TextureCreateInfo m_CreateInfo;
		bool m_BadImport = false;

	public:
		constexpr static auto properties = std::make_tuple(
			property(&TextureImporter::m_Uuid, "UUID"),
			property(&TextureImporter::m_CreateInfo, "CreateInfo")
		);
	};
}
