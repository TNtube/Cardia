#include "Cardia/Renderer/Renderer.hpp"

namespace Cardia
{
	template<>
	inline std::shared_ptr<Texture> AssetsManager::Load(const AssetHandle& handle)
	{
		if (m_Assets.contains(handle)) {
			return std::static_pointer_cast<Texture>(m_Assets.at(handle).Resource);
		}
		auto path = AbsolutePathFromHandle(handle);
		std::shared_ptr<Texture> texture = Texture::Builder(m_Renderer.GetDevice())
			.SetAssetHandle(handle)
			.Build();

		m_Assets[handle] = AssetRefCounter(texture);

		return texture;
	}

	template<>
	inline std::shared_ptr<Material> AssetsManager::Load(const AssetHandle& handle)
	{
		if (m_Assets.contains(handle)) {
			return std::static_pointer_cast<Material>(m_Assets.at(handle).Resource);
		}
		auto path = AbsolutePathFromHandle(handle);
		auto materialData = Serializer<MaterialData>::Deserialize(path);
		if (!materialData) {
			return nullptr;
		}

		auto material = std::make_shared<Material>(m_Renderer.GetDevice(), *materialData, handle);
		m_Assets[handle] = AssetRefCounter(material);

		return material;
	}

	template<>
	inline std::shared_ptr<MeshRenderer> AssetsManager::Load(const AssetHandle& handle)
	{
		if (m_Assets.contains(handle)) {
			return std::static_pointer_cast<MeshRenderer>(m_Assets.at(handle).Resource);
		}
		auto path = AbsolutePathFromHandle(handle);
		auto materialData = Model::FromFile(path, true);

		auto material = std::make_shared<MeshRenderer>(m_Renderer.GetDevice(), handle, materialData);
		m_Assets[handle] = AssetRefCounter(material);

		return material;
	}
}