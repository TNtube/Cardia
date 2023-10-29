#pragma once

#include <memory>
#include <string>
#include <type_traits>
#include <queue>
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
		bool Dirty = false;
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

		AssetHandle GetHandleFromRelative(const std::filesystem::path& relativePath);
		AssetHandle GetHandleFromAbsolute(const std::filesystem::path& absolutePath);
		AssetHandle GetHandleFromAsset(const std::filesystem::path& relativeToAssetsPath);
		AssetHandle AddEntry(const std::filesystem::path& absolutePath);

		void SetDirty(const AssetHandle& handle);
		bool IsDirty(const AssetHandle& handle) const;
		void ReloadAllDirty();

		void PopulateHandleFromProject(const Project& project);
		void PopulateHandleFromResource();

		void ReloadAssetFromHandle(const AssetHandle& handle);

		std::filesystem::path RelativePathFromHandle(const AssetHandle& handle) const;
		std::filesystem::path AbsolutePathFromHandle(const AssetHandle& handle) const;

		void Update();

	private:
		void PopulateHandleFromPath(const std::filesystem::path& abs);
		bool LoadHandleFromPath(const std::filesystem::path& abs);
		void RegisterNewHandle(const std::filesystem::path& abs);

		void RemovePathForHandle(const AssetHandle& handle);

		void CollectUnusedAssets(bool force = false);

		const Renderer& m_Renderer;

		Project m_Project;

		std::unordered_map<AssetHandle, AssetRefCounter> m_Assets;
		std::unordered_map<std::filesystem::path, AssetHandle> m_AssetPaths;


		// file watcher
		struct FileUpdateInfo {
			std::filesystem::path OldPath;
			std::filesystem::path NewPath;
			efsw::Action Action;
		};

		class AssetsListener : public efsw::FileWatchListener
		{
		public:
			explicit AssetsListener(AssetsManager& assetsManager) : m_AssetsManager(assetsManager) {}
			void handleFileAction(efsw::WatchID watchId, const std::string& dir, const std::string& filename,
								  efsw::Action action, std::string oldFilename) override;

			void WatcherRoutine();
		private:
			void OnFileAdded(const std::filesystem::path& newPath);
			void OnFileRemoved(const std::filesystem::path& newPath);
			void OnFileUpdate(const std::filesystem::path& newPath);
			void OnFileRename(const std::filesystem::path& oldPath, const std::filesystem::path& newPath);

			void ComputeFileInfo(FileUpdateInfo& updateInfo);

			AssetsManager& m_AssetsManager;
			std::queue<FileUpdateInfo> m_Queue;
			std::mutex m_WatcherMutex;
		};

		AssetsListener m_AssetsListener;

		efsw::FileWatcher m_FileWatcher;
		efsw::WatchID m_WatchID = 0;
	};
}

#include "Cardia/Asset/AssetsManager.inl"
