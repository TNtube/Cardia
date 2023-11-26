#include "Cardia/Renderer/Renderer.hpp"

namespace Cardia
{
	template<typename T>
	inline std::shared_ptr<T> AssetsManager::Load(const std::filesystem::path& path)
	{
		return Load<T>(GetHandleFromAsset(path));
	}

	template<typename T>
	inline std::shared_ptr<T> AssetsManager::Load(const AssetHandle& handle)
	{
		if (!m_Assets.contains(handle))	return nullptr;

		auto& assetData = m_Assets.at(handle);
		if (!assetData.Importer->IsTypeValid<T>()) return nullptr;

		assetData.Importer->Import(assetData.Resource);
		return std::static_pointer_cast<T>(assetData.Resource);
	}
}