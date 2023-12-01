#include "cdpch.hpp"

#include <Cardia/Project/Project.hpp>
#include <Cardia/Renderer/Renderer.hpp>
#include <Cardia/Application.hpp>
#include "Cardia/Assets/AssetsManager.hpp"

#include <ranges>

constexpr std::chrono::duration<float> GC_COLLECTION_DURATION = std::chrono::duration<float>(2.0f);
constexpr std::uint32_t MAX_UNUSED_COUNT = 2;

namespace Cardia
{
	AssetsManager::AssetsManager(const Renderer& renderer) : m_Renderer(renderer), m_AssetsListener(*this) {
		WalkAssetsFromResource();
	}

	UUID AssetsManager::GetUUIDFromAbsolute(const std::filesystem::path& absolutePath)
	{
		const auto normalized = std::filesystem::absolute(absolutePath);
		if (m_AssetPaths.contains(normalized))
			return m_AssetPaths[normalized];

		try
		{
			const auto id = Serializer<UUID>::Deserialize(normalized.string() + ".imp");
			m_AssetPaths[normalized] = id;
			return m_AssetPaths[normalized];
		}catch (std::exception& /* e */)
		{
			Log::Error("Failed to load asset at {} : .imp file missing or invalid.\nPlease reimport it.", normalized.string());
			return UUID::Default();
		}
	}

	UUID AssetsManager::GetUUIDFromAsset(const std::filesystem::path& relativeToAssetsPath)
	{
		const auto absolute = m_Project.ProjectPath() / m_Project.GetConfig().AssetDirectory / relativeToAssetsPath;
		return GetUUIDFromAbsolute(absolute);
	}

	UUID AssetsManager::GetUUIDFromRelative(const std::filesystem::path& relativePath)
	{
		if (relativePath.is_relative()) {
			return GetUUIDFromAbsolute(std::filesystem::absolute(relativePath));
		}
		return GetUUIDFromAbsolute(relativePath);
	}

	void AssetsManager::WalkAssetsFromProject(const Project& project)
	{
		m_Project = project;
		auto& config = m_Project.GetConfig();
		const auto absoluteAssetsPath = m_Project.ProjectPath() / config.AssetDirectory;
		WalkAssetsFromPath(absoluteAssetsPath);

		m_FileWatcher.removeWatch(m_WatchID);
		m_WatchID = m_FileWatcher.addWatch(absoluteAssetsPath.string(),& m_AssetsListener, true);
		m_FileWatcher.watch();
	}

	void AssetsManager::WalkAssetsFromResource()
	{
		WalkAssetsFromPath("resources");
	}

	std::filesystem::path AssetsManager::RelativePathFromUUID(const UUID& id) const
	{
		const auto path = AbsolutePathFromUUID(id);
		if (!path.empty())
			return std::filesystem::relative(path, m_Project.ProjectPath() / m_Project.GetConfig().AssetDirectory);

		return {};
	}

	std::filesystem::path AssetsManager::AbsolutePathFromUUID(const UUID& id) const
	{
		for (const auto& [path, h] : m_AssetPaths) {
			if (h == id)
				return path;
		}
		return {};
	}

	UUID AssetsManager::AddPathEntry(const std::filesystem::path& absolutePath)
	{
		UUID newUuid;
		m_AssetPaths[std::filesystem::absolute(absolutePath)] = newUuid;
		return newUuid;
	}

	void AssetsManager::WalkAssetsFromPath(const std::filesystem::path& abs)
	{
		std::unordered_set<std::filesystem::path> alreadyVisited;

		for (const auto& entry : std::filesystem::recursive_directory_iterator(abs)) {
			auto path = std::filesystem::absolute(entry.path());
			if (path.extension() == ".imp") {
				if (LoadAssetUUIDFromPath(path))
					alreadyVisited.insert(path);
				continue;
			}

			const auto& impPath = std::filesystem::absolute(entry.path().string() + ".imp");

			if (alreadyVisited.contains(impPath) || std::filesystem::exists(impPath))
				continue;

			RegisterNewAssetsUUID(path);
		}
	}

	bool AssetsManager::LoadAssetUUIDFromPath(const std::filesystem::path& abs)
	{
		auto path = std::filesystem::absolute(abs);
		try
		{
			const auto id = Serializer<UUID>::Deserialize(path);
			m_AssetPaths[path.replace_extension()] = id;
			return true;
		}
		catch (std::exception& /* e */)
		{
			return false;
		}
	}

	void AssetsManager::RegisterNewAssetsUUID(const std::filesystem::path& assetPath)
	{
		UUID id;
		m_AssetPaths[std::filesystem::absolute(assetPath)] = id;

		Serializer serializer(id);
		serializer.Serialize(assetPath.string() + ".imp");
	}

	void AssetsManager::ReloadAssetFromUUID(const UUID& id)
	{
		const auto it = m_Assets.find(id);
		if (it == m_Assets.end())
			return;

		const auto& asset = it->second;
		// nuh uh
		// const auto assetPtr = std::static_pointer_cast<Asset>(asset.Resource);
		// assetPtr->Reload();
	}

	void AssetsManager::RemovePathForUUID(const UUID& id)
	{
		for (auto it = m_AssetPaths.begin(); it != m_AssetPaths.end(); ++it) {
			if (it->second == id) {
				m_AssetPaths.erase(it);
				return;
			}
		}
	}

	void AssetsManager::ReloadAllDirty() {
		for (auto& asset : m_Assets | std::views::values) {
			if (asset.Importer->IsDirty()) {
				asset.Importer->Import(asset.Resource);
			}
		}
	}

	void AssetsManager::SetDirty(const UUID& id)
	{
		const auto it = m_Assets.find(id);
		if (it == m_Assets.end())
			return;

		it->second.Importer->SetDirty();
	}

	void AssetsManager::Update()
	{
		if (!Application::Get().GetWindow().IsFocused()) return;

		m_AssetsListener.WatcherRoutine();
//		CollectUnusedAssets();
	}

	void AssetsManager::CollectUnusedAssets(const bool force)
	{
		static auto lastCollection = std::chrono::steady_clock::now();

		if (!force && std::chrono::steady_clock::now() - lastCollection < GC_COLLECTION_DURATION)
			return;

		for (auto it = m_Assets.begin(); it != m_Assets.end();) {

			if (it->second.Resource.use_count() == 1) {
				++it->second.UnusedCounter;
			}

			if (force || it->second.UnusedCounter > MAX_UNUSED_COUNT) {
				auto path = AbsolutePathFromUUID(it->first);
				Log::Info("Unloading asset at {}", path.string());
				it = m_Assets.erase(it);
			}
			else {
				++it;
			}
		}

		lastCollection = std::chrono::steady_clock::now();
	}

	bool AssetsManager::IsDirty(const UUID &id) const
	{
		const auto it = m_Assets.find(id);
		if (it == m_Assets.end())
			return false;

		return it->second.Importer->IsDirty();
	}

	UUID AssetsManager::AddAssetEntry(const std::shared_ptr<void>& asset)
	{
		UUID newUuid;
		m_Assets[newUuid] = AssetData(asset);
		return newUuid;
	}

	void AssetsManager::AssetsListener::handleFileAction(efsw::WatchID watchId, const std::string& dir,
														 const std::string& filename, efsw::Action action,
														 const std::string oldFilename)
	{
		std::scoped_lock<std::mutex> lock(m_WatcherMutex);

		FileUpdateInfo& info = m_Queue.emplace();

		info.Action = action;

		info.NewPath = dir;
		info.NewPath /= filename;

		if (!oldFilename.empty()) {
			info.OldPath = dir;
			info.OldPath /= oldFilename;
		}
	}

	void AssetsManager::AssetsListener::WatcherRoutine() {
		{
			// nested scope to unlock mutex as soon as possible
			std::scoped_lock<std::mutex> lock(m_WatcherMutex);

			while (!m_Queue.empty()) {
				auto& info = m_Queue.front();
				ComputeFileInfo(info);
				m_Queue.pop();
			}
		}

		m_AssetsManager.ReloadAllDirty();
	}

	void AssetsManager::AssetsListener::ComputeFileInfo(const AssetsManager::FileUpdateInfo& updateInfo) const
	{
		switch (updateInfo.Action) {
			case efsw::Actions::Add: OnFileAdded(updateInfo.NewPath);
				break;
			case efsw::Actions::Delete:	OnFileRemoved(updateInfo.NewPath);
				break;
			case efsw::Actions::Modified: OnFileUpdate(updateInfo.NewPath);
				break;
			case efsw::Actions::Moved: OnFileRename(updateInfo.OldPath, updateInfo.NewPath);
				break;
		}

	}

	void AssetsManager::AssetsListener::OnFileAdded(const std::filesystem::path& newPath) const
	{
		if (newPath.extension() == ".imp")
		{
			m_AssetsManager.LoadAssetUUIDFromPath(newPath);
			return;
		}

		const auto impPath = newPath.string() + ".imp";

		if (std::filesystem::exists(impPath))
			return;

		m_AssetsManager.RegisterNewAssetsUUID(newPath);
	}

	void AssetsManager::AssetsListener::OnFileRemoved(const std::filesystem::path& newPath) const
	{
		auto finalPath = newPath;

		if (finalPath.extension() == ".imp") {
			finalPath.replace_extension();
		}

		const auto id = m_AssetsManager.GetUUIDFromAbsolute(finalPath);

		m_AssetsManager.SetDirty(id);
		m_AssetsManager.RemovePathForUUID(id);
	}

	void AssetsManager::AssetsListener::OnFileUpdate(const std::filesystem::path& newPath) const
	{
		if (newPath.extension() == ".imp") {
			auto finalPath = newPath;
			finalPath.replace_extension();

			const auto oldUuid = m_AssetsManager.GetUUIDFromAbsolute(finalPath);

			m_AssetsManager.SetDirty(oldUuid);
			m_AssetsManager.RemovePathForUUID(oldUuid);

			m_AssetsManager.LoadAssetUUIDFromPath(finalPath);
			return;
		}

		const auto id = m_AssetsManager.GetUUIDFromAbsolute(newPath);
		m_AssetsManager.SetDirty(id);
	}

	void AssetsManager::AssetsListener::OnFileRename(const std::filesystem::path& oldPath,
	                                                 const std::filesystem::path& newPath) const
	{
		auto finalNewPath = std::filesystem::absolute(newPath);

		if (oldPath.extension() == ".imp") {
			auto finalOldPath = oldPath;
			finalOldPath.replace_extension();

			const auto oldUuid = m_AssetsManager.GetUUIDFromAbsolute(finalOldPath);

			m_AssetsManager.RemovePathForUUID(oldUuid);

			if (newPath.extension() == ".imp") {
				finalNewPath.replace_extension();
				m_AssetsManager.m_AssetPaths[finalNewPath] = oldUuid;
			}
			return;
		}

		const auto oldUuid = m_AssetsManager.GetUUIDFromAbsolute(oldPath);
		m_AssetsManager.RemovePathForUUID(oldUuid);
		m_AssetsManager.m_AssetPaths[finalNewPath] = oldUuid;
	}
}
