#pragma once

#include <memory>
#include <string>
#include <type_traits>
#include "Cardia/Serialization/Serializer.hpp"
#include "Cardia/Project/Project.hpp"
#include "Cardia/Renderer/Material.hpp"
#include "Cardia/Renderer/MeshRenderer.hpp"
#include "Cardia/Renderer/Texture.hpp"

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
	template <typename T> concept AssetType = std::is_base_of_v<Asset, T>;

	class AssetsManager
	{
	public:
		explicit AssetsManager(const Renderer& renderer) : m_Renderer(renderer) {}

		template<AssetType T>
		std::shared_ptr<T> Load(const std::filesystem::path& path)
		{
			return Load<T>(GetHandleFromRelative(path));
		}

		template<AssetType T>
		std::shared_ptr<T> Load(const AssetHandle& handle)
		{
			CdCoreAssert(false, "Asset type not supported");
		}

		template<>
		std::shared_ptr<Texture> Load(const AssetHandle& handle);

		template<>
		std::shared_ptr<MeshRenderer> Load(const AssetHandle& handle);

		template<>
		std::shared_ptr<Material> Load(const AssetHandle& handle);

		template<>
		std::shared_ptr<Shader> Load(const AssetHandle& handle) { return nullptr; }

		virtual AssetHandle GetHandleFromRelative(const std::filesystem::path& relativePath) {

			auto absolute = m_Project.ProjectPath() / m_Project.GetConfig().AssetDirectory / relativePath;
			return GetHandleFromAbsolute(absolute);
		}

		AssetHandle GetHandleFromAbsolute(const std::filesystem::path& absolutePath) {
			if (!m_AssetPaths.contains(absolutePath))
				return m_AssetPaths[absolutePath];


			auto handle = Serializer<AssetHandle>::Deserialize(absolutePath.string() + ".imp");
			if (!handle)
			{
				Log::Error("Failed to load asset at {} : .imp file missing or invalid.\nPlease reimport it.", absolutePath.string());
				return AssetHandle::Invalid();
			}

			m_AssetPaths[absolutePath] = *handle;
			return m_AssetPaths[absolutePath];
		}

		AssetHandle AddEntry(const std::filesystem::path& absolutePath);

		void PopulateHandleFromProject(const Project& project);

		std::filesystem::path RelativePathFromHandle(const AssetHandle& handle) const;
		std::filesystem::path AbsolutePathFromHandle(const AssetHandle& handle) const;

	private:
		const Renderer& m_Renderer;

		Project m_Project;

		std::unordered_map<AssetHandle, AssetRefCounter> m_Assets;
		std::unordered_map<std::filesystem::path, AssetHandle> m_AssetPaths;
	};
}

#include "Cardia/Asset/AssetsManager.inl"
