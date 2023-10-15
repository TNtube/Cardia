#pragma once

#include <memory>
#include <string>
#include <type_traits>
#include <efsw/efsw.hpp>
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
		explicit AssetsManager(const Renderer& renderer);

		template<AssetType T> std::shared_ptr<T> Load(const std::filesystem::path& path);
		template<AssetType T> std::shared_ptr<T> Load(const AssetHandle& handle);
		template<> std::shared_ptr<Texture>      Load(const AssetHandle& handle);
		template<> std::shared_ptr<MeshRenderer> Load(const AssetHandle& handle);
		template<> std::shared_ptr<Material>     Load(const AssetHandle& handle);
		template<> std::shared_ptr<Shader>       Load(const AssetHandle& handle);

		AssetHandle GetHandleFromRelative(const std::filesystem::path& relativePath);
		AssetHandle GetHandleFromAbsolute(const std::filesystem::path& absolutePath);
		AssetHandle GetHandleFromAsset(const std::filesystem::path& relativeToAssetsPath);
		AssetHandle AddEntry(const std::filesystem::path& absolutePath);

		void PopulateHandleFromProject(const Project& project);
		void PopulateHandleFromResource();

		std::filesystem::path RelativePathFromHandle(const AssetHandle& handle) const;
		std::filesystem::path AbsolutePathFromHandle(const AssetHandle& handle) const;

	private:
		void PopulateHandleFromPath(const std::filesystem::path& abs);
		const Renderer& m_Renderer;

		Project m_Project;

		std::unordered_map<AssetHandle, AssetRefCounter> m_Assets;
		std::unordered_map<std::filesystem::path, AssetHandle> m_AssetPaths;


		// file watcher

		class AssetsListener : public efsw::FileWatchListener
		{
		public:
			explicit AssetsListener(AssetsManager& assetsManager) : m_AssetsManager(assetsManager) {}
			void handleFileAction(efsw::WatchID watchId, const std::string& dir, const std::string& filename,
								  efsw::Action action, std::string oldFilename) override;
		private:
			void OnFileAdded(efsw::WatchID watchId, const std::string& dir, const std::string& filename);
			void OnFileRemoved(efsw::WatchID watchId, const std::string& dir, const std::string& filename);
			void OnFileUpdate(efsw::WatchID watchId, const std::string& dir, const std::string& filename);
			void OnFileRename(efsw::WatchID watchId, const std::string& dir, const std::string& oldFilename, const std::string& newFilename);

			AssetsManager& m_AssetsManager;
		};

		AssetsListener m_AssetsListener;

		efsw::FileWatcher m_FileWatcher;

		efsw::WatchID m_WatchID = 0;
	};
}

#include "Cardia/Asset/AssetsManager.inl"
