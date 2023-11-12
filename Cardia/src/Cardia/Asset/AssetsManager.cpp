#include "cdpch.hpp"

#include <Cardia/Project/Project.hpp>
#include <Cardia/Renderer/Renderer.hpp>
#include <Cardia/Application.hpp>
#include "Cardia/Asset/AssetsManager.hpp"

#include <ranges>

constexpr std::chrono::duration<float> GC_COLLECTION_DURATION = std::chrono::duration<float>(2.0f);
constexpr std::uint32_t MAX_UNUSED_COUNT = 2;

namespace Cardia
{
	AssetsManager::AssetsManager(const Renderer& renderer) : m_Renderer(renderer), m_AssetsListener(*this) {
		PopulateHandleFromResource();
	}

	AssetHandle AssetsManager::GetHandleFromAbsolute(const std::filesystem::path& absolutePath)
	{
		const auto normalized = std::filesystem::absolute(absolutePath);
		if (m_AssetPaths.contains(normalized))
			return m_AssetPaths[normalized];

		const auto handle = Serializer<AssetHandle>::Deserialize(normalized.string() + ".imp");
		if (!handle)
		{
			Log::Error("Failed to load asset at {} : .imp file missing or invalid.\nPlease reimport it.", normalized.string());
			return AssetHandle::Invalid();
		}

		m_AssetPaths[normalized] = *handle;
		return m_AssetPaths[normalized];
	}

	AssetHandle AssetsManager::GetHandleFromAsset(const std::filesystem::path& relativeToAssetsPath)
	{
		const auto absolute = m_Project.ProjectPath() / m_Project.GetConfig().AssetDirectory / relativeToAssetsPath;
		return GetHandleFromAbsolute(absolute);
	}

	AssetHandle AssetsManager::GetHandleFromRelative(const std::filesystem::path& relativePath)
	{
		if (relativePath.is_relative()) {
			return GetHandleFromAbsolute(std::filesystem::absolute(relativePath));
		}
		return GetHandleFromAbsolute(relativePath);
	}

	void AssetsManager::PopulateHandleFromProject(const Project& project)
	{
		m_Project = project;
		auto& config = m_Project.GetConfig();
		const auto absoluteAssetsPath = m_Project.ProjectPath() / config.AssetDirectory;
		PopulateHandleFromPath(absoluteAssetsPath);

		m_FileWatcher.removeWatch(m_WatchID);
		m_WatchID = m_FileWatcher.addWatch(absoluteAssetsPath.string(),& m_AssetsListener, true);
		m_FileWatcher.watch();
	}

	void AssetsManager::PopulateHandleFromResource()
	{
		PopulateHandleFromPath("resources");
	}

	std::filesystem::path AssetsManager::RelativePathFromHandle(const AssetHandle& handle) const
	{
		const auto path = AbsolutePathFromHandle(handle);
		if (!path.empty())
			return std::filesystem::relative(path, m_Project.ProjectPath() / m_Project.GetConfig().AssetDirectory);

		return {};
	}

	std::filesystem::path AssetsManager::AbsolutePathFromHandle(const AssetHandle& handle) const
	{
		for (const auto& [path, h] : m_AssetPaths) {
			if (h == handle)
				return path;
		}
		return {};
	}

	AssetHandle AssetsManager::AddPathEntry(const std::filesystem::path& absolutePath)
	{
		AssetHandle newHandle;
		m_AssetPaths[std::filesystem::absolute(absolutePath)] = newHandle;
		return newHandle;
	}

	void AssetsManager::PopulateHandleFromPath(const std::filesystem::path& abs)
	{
		std::unordered_set<std::filesystem::path> alreadyVisited;

		for (const auto& entry : std::filesystem::recursive_directory_iterator(abs)) {
			auto path = std::filesystem::absolute(entry.path());
			if (path.extension() == ".imp") {
				if (LoadHandleFromPath(path))
					alreadyVisited.insert(path);
				continue;
			}

			const auto& impPath = std::filesystem::absolute(entry.path().string() + ".imp");

			if (alreadyVisited.contains(impPath) || std::filesystem::exists(impPath))
				continue;

			RegisterNewHandle(path);
		}
	}

	bool AssetsManager::LoadHandleFromPath(const std::filesystem::path& abs)
	{
		auto path = std::filesystem::absolute(abs);
		const auto handle = Serializer<AssetHandle>::Deserialize(path);
		if (handle) {
			m_AssetPaths[path.replace_extension()] = *handle;
			return true;
		}
		return false;
	}

	void AssetsManager::RegisterNewHandle(const std::filesystem::path& assetPath)
	{
		AssetHandle handle;
		m_AssetPaths[std::filesystem::absolute(assetPath)] = handle;

		Serializer<AssetHandle> serializer(handle);
		serializer.Serialize(assetPath.string() + ".imp");
	}

	void AssetsManager::ReloadAssetFromHandle(const AssetHandle& handle)
	{
		const auto it = m_Assets.find(handle);
		if (it == m_Assets.end())
			return;

		const auto& asset = it->second;
		const auto assetPtr = std::static_pointer_cast<Asset>(asset.Resource);
		assetPtr->Reload();
	}

	void AssetsManager::RemovePathForHandle(const AssetHandle& handle)
	{
		for (auto it = m_AssetPaths.begin(); it != m_AssetPaths.end(); ++it) {
			if (it->second == handle) {
				m_AssetPaths.erase(it);
				return;
			}
		}
	}

	void AssetsManager::ReloadAllDirty() {
		if (!std::ranges::any_of(m_Assets, [](const auto& pair) { return pair.second.Dirty; })) {
			return;
		}
		for (auto& asset : m_Assets | std::views::values) {
			const auto assetPtr = std::static_pointer_cast<Asset>(asset.Resource);
			if (asset.Dirty) {
				assetPtr->Reload();
				asset.Dirty = false;
			} else {
				asset.Dirty = assetPtr->CheckForDirtyInDependencies();
			}
		}

		for (auto& asset: m_Assets | std::views::values) {
			if (asset.Dirty) {
				const auto assetPtr = std::static_pointer_cast<Asset>(asset.Resource);
				assetPtr->Reload();
				asset.Dirty = false;
			}
		}
	}

	void AssetsManager::SetDirty(const AssetHandle &handle)
	{
		const auto it = m_Assets.find(handle);
		if (it == m_Assets.end())
			return;

		it->second.Dirty = true;
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
				auto path = AbsolutePathFromHandle(it->first);
				Log::Info("Unloading asset at {}", path.string());
				it = m_Assets.erase(it);
			}
			else {
				++it;
			}
		}

		lastCollection = std::chrono::steady_clock::now();
	}

	bool AssetsManager::IsDirty(const AssetHandle &handle) const
	{
		const auto it = m_Assets.find(handle);
		if (it == m_Assets.end())
			return false;

		return it->second.Dirty;
	}

	void AssetsManager::AddAssetEntry(const std::shared_ptr<Asset>& asset)
	{
		m_Assets[asset->GetHandle()] = AssetRefCounter(asset);
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
			m_AssetsManager.LoadHandleFromPath(newPath);
			return;
		}

		const auto impPath = newPath.string() + ".imp";

		if (std::filesystem::exists(impPath))
			return;

		m_AssetsManager.RegisterNewHandle(newPath);
	}

	void AssetsManager::AssetsListener::OnFileRemoved(const std::filesystem::path& newPath) const
	{
		auto finalPath = newPath;

		if (finalPath.extension() == ".imp") {
			finalPath.replace_extension();
		}

		const auto handle = m_AssetsManager.GetHandleFromAbsolute(finalPath);

		m_AssetsManager.SetDirty(handle);
		m_AssetsManager.RemovePathForHandle(handle);
	}

	void AssetsManager::AssetsListener::OnFileUpdate(const std::filesystem::path& newPath) const
	{
		if (newPath.extension() == ".imp") {
			auto finalPath = newPath;
			finalPath.replace_extension();

			const auto oldHandle = m_AssetsManager.GetHandleFromAbsolute(finalPath);

			m_AssetsManager.SetDirty(oldHandle);
			m_AssetsManager.RemovePathForHandle(oldHandle);

			m_AssetsManager.LoadHandleFromPath(finalPath);
			return;
		}

		const auto handle = m_AssetsManager.GetHandleFromAbsolute(newPath);
		m_AssetsManager.SetDirty(handle);
	}

	void AssetsManager::AssetsListener::OnFileRename(const std::filesystem::path& oldPath,
	                                                 const std::filesystem::path& newPath) const
	{
		auto finalNewPath = std::filesystem::absolute(newPath);

		if (oldPath.extension() == ".imp") {
			auto finalOldPath = oldPath;
			finalOldPath.replace_extension();

			const auto oldHandle = m_AssetsManager.GetHandleFromAbsolute(finalOldPath);

			m_AssetsManager.RemovePathForHandle(oldHandle);

			if (newPath.extension() == ".imp") {
				finalNewPath.replace_extension();
				m_AssetsManager.m_AssetPaths[finalNewPath] = oldHandle;
			}
			return;
		}

		const auto oldHandle = m_AssetsManager.GetHandleFromAbsolute(oldPath);
		m_AssetsManager.RemovePathForHandle(oldHandle);
		m_AssetsManager.m_AssetPaths[finalNewPath] = oldHandle;
	}
}
