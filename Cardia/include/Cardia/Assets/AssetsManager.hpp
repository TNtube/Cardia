#pragma once

#include <memory>
#include <string>
#include <type_traits>
#include <queue>
#include <efsw/efsw.hpp>
#include "Cardia/Serialization/Serializer.hpp"
#include "Cardia/Project/Project.hpp"
#include "Cardia/Renderer/Material.hpp"
#include "Cardia/Renderer/Texture.hpp"
#include "Importers/Importer.hpp"

namespace Cardia {
	struct AssetData
	{
		AssetData() = default;
		explicit AssetData(std::shared_ptr<Importer> importer) : Importer(std::move(importer)) {}
		std::shared_ptr<Importer> Importer;
		std::shared_ptr<void> Resource;
		uint32_t UnusedCounter = 0;
	};
}

namespace Cardia
{
	class AssetsManager
	{
	public:
		explicit AssetsManager(const Renderer& renderer);

		template<typename T> std::shared_ptr<T> Load(const std::filesystem::path& path);
		template<typename T> std::shared_ptr<T> Load(const UUID& id);

		UUID GetUUIDFromRelative(const std::filesystem::path& relativePath);
		UUID GetUUIDFromAbsolute(const std::filesystem::path& absolutePath);
		UUID GetUUIDFromAsset(const std::filesystem::path& relativeToAssetsPath);
		UUID AddPathEntry(const std::filesystem::path& absolutePath);
		UUID AddAssetEntry(const std::shared_ptr<void>& asset);

		void SetDirty(const UUID& id);
		bool IsDirty(const UUID& id) const;
		void ReloadAllDirty();

		void WalkAssetsFromProject(const Project& project);
		void WalkAssetsFromResource();

		void ReloadAssetFromUUID(const UUID& id);

		std::filesystem::path RelativePathFromUUID(const UUID& id) const;
		std::filesystem::path AbsolutePathFromUUID(const UUID& id) const;

		void Update();

	private:
		void WalkAssetsFromPath(const std::filesystem::path& abs);
		bool LoadAssetDataFromPath(const std::filesystem::path& abs);
		void RegisterNewAsset(const std::filesystem::path& abs);

		void RemovePathForUUID(const UUID& id);

		void CollectUnusedAssets(bool force = false);

		const Renderer& m_Renderer;

		Project m_Project;

		std::unordered_map<UUID, AssetData> m_Assets;
		std::unordered_map<std::filesystem::path, UUID> m_AssetPaths;


		// file watcher
		struct FileUpdateInfo {
			std::filesystem::path OldPath;
			std::filesystem::path NewPath;
			efsw::Action Action;
		};

		class AssetsListener final : public efsw::FileWatchListener
		{
		public:
			explicit AssetsListener(AssetsManager& assetsManager) : m_AssetsManager(assetsManager) {}
			void handleFileAction(efsw::WatchID watchId, const std::string& dir, const std::string& filename,
								  efsw::Action action, std::string oldFilename) override;

			void WatcherRoutine();
		private:
			void OnFileAdded(const std::filesystem::path& newPath) const;
			void OnFileRemoved(const std::filesystem::path& newPath) const;
			void OnFileUpdate(const std::filesystem::path& newPath) const;
			void OnFileRename(const std::filesystem::path& oldPath, const std::filesystem::path& newPath) const;

			void ComputeFileInfo(const FileUpdateInfo& updateInfo) const;

			AssetsManager& m_AssetsManager;
			std::queue<FileUpdateInfo> m_Queue;
			std::mutex m_WatcherMutex;
		};

		AssetsListener m_AssetsListener;

		efsw::FileWatcher m_FileWatcher;
		efsw::WatchID m_WatchID = 0;
	};
}

#include "Cardia/Assets/AssetsManager.inl"
