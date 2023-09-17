#pragma once

#include <memory>
#include <string>
#include <type_traits>
#include "Cardia/Serialization/Serializer.hpp"

namespace Cardia {
	struct AssetRefCounter
	{
		AssetRefCounter() = default;
		explicit AssetRefCounter(std::shared_ptr<void> resource) : Resource(std::move(resource)) {}
		std::shared_ptr<void> Resource;
		uint32_t UnusedCounter = 0;
	};
}

namespace Cardia
{
	template <typename T> concept AssetType = std::same_as<T, Asset>;

	class AssetsManager
	{
	public:
		AssetsManager(const Renderer& renderer) : m_Renderer(renderer) {}

		template<AssetType T>
		std::shared_ptr<T> Load(const std::filesystem::path& path)
		{
			return Load<T>(GetHandle(path));
		}

		template<AssetType T>
		std::shared_ptr<T> Load(const AssetHandle& handle)
		{
			CdCoreAssert(false, "Asset type not supported");
		}

		template<AssetType T>
		std::shared_ptr<Texture> Load(const AssetHandle& handle);

		template<AssetType T>
		std::shared_ptr<Shader> Load(const AssetHandle& handle);

		template<AssetType T>
		std::shared_ptr<Material> Load(const AssetHandle& handle);

//		template<AssetType T>
//		std::shared_ptr<Model> Load(const AssetHandle& handle);

		virtual AssetHandle GetHandle(const std::filesystem::path& relativePath) {
			if (!m_AssetPaths.contains(relativePath))
				return m_AssetPaths[relativePath];

			auto handle = Serializer<AssetHandle>::Deserialize(GetAssetPath(relativePath));
			if (!handle)
			{
				Log::Error("Failed to load asset at {} : .imp file missing or invalid.\nPlease reimport it.", relativePath.string());
				return AssetHandle::Invalid();
			}

			m_AssetPaths[relativePath] = *handle;
			return m_AssetPaths[relativePath];
		}

		virtual std::filesystem::path GetAssetPath(const std::filesystem::path& path) = 0;

	private:
		const Renderer& m_Renderer;

		std::unordered_map<AssetHandle, AssetRefCounter> m_Assets;
		std::unordered_map<std::filesystem::path, AssetHandle> m_AssetPaths;

	};
}

#include "Cardia/Asset/AssetsManager.inl"
