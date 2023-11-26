#pragma once
#include "Importer.hpp"
#include "Cardia/Renderer/Texture.hpp"


namespace Cardia
{
	class AssetsManager;
	class TextureImporter : public Importer
	{
	public:
		explicit TextureImporter(AssetHandle handle)
		{
			m_Handle = std::move(handle);
		}
		void Import(std::shared_ptr<void>& asset) override;
		AssetType GetType() override;

	private:
		TextureCreateInfo m_CreateInfo;
		bool m_BadImport = false;

	public:
		constexpr static auto properties = std::make_tuple(
			property(&TextureImporter::m_CreateInfo, "CreateInfo")
		);
	};
}
