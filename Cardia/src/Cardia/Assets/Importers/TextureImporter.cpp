#include "Cardia/Assets/Importers/TextureImporter.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>

#include "Cardia/Application.hpp"

namespace Cardia
{
	void TextureImporter::Import(std::shared_ptr<void>& asset)
	{
		if (!m_Dirty)
			return;

		auto& app = Application::Get();

		if (!asset)
			asset = std::make_shared<Texture>(app.GetRenderer().GetDevice(), m_CreateInfo);

		std::string strPath = app.GetAssetsManager().AbsolutePathFromHandle(m_Handle).string();
		int texWidth {}, texHeight {}, texChannels {};

		stbi_uc* pixels = stbi_load(strPath.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

		unsigned char errorColor[] = {255, 0, 255, 255};
		m_BadImport = false;
		if (!pixels)
		{
			auto* error = stbi_failure_reason();
			Log::CoreError("Error: `{0}` at path `{1}`", error, strPath);
			texWidth = 1;
			texHeight = 1;
			pixels = errorColor;

			m_BadImport = true;
		}

		const std::size_t size = texWidth * texHeight * texChannels;
		const std::vector<std::uint8_t> data {pixels, pixels + size};

		const auto textureAsset = std::static_pointer_cast<Texture>(asset);
		textureAsset->SetCreateInfo(m_CreateInfo);
		textureAsset->SetData(data, {static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight)});

		if (!m_BadImport)
			stbi_image_free(pixels);

		m_Dirty = false;
	}

	AssetType TextureImporter::GetType()
	{
		return AssetType::Texture;
	}
}
