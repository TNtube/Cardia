#include "cdpch.hpp"

#include <Cardia/Project/Project.hpp>
#include <Cardia/Renderer/Renderer.hpp>
#include "Cardia/Asset/AssetsManager.hpp"

constexpr std::chrono::duration<float> GC_COLLECTION_DURATION = std::chrono::duration<float>(2.0f);
constexpr std::uint32_t MAX_UNUSED_COUNT = 2;

namespace Cardia
{
	AssetsManager::AssetsManager(const Renderer& renderer) : m_Renderer(renderer), m_AssetsListener(*this) {
		PopulateHandleFromResource();
	}

	AssetHandle AssetsManager::GetHandleFromAbsolute(const std::filesystem::path& absolutePath)
	{
		auto normalized = std::filesystem::absolute(absolutePath);
		if (m_AssetPaths.contains(normalized))
			return m_AssetPaths[normalized];

		auto handle = Serializer<AssetHandle>::Deserialize(normalized.string() + ".imp");
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
		auto absolute = m_Project.ProjectPath() / m_Project.GetConfig().AssetDirectory / relativeToAssetsPath;
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
		auto absoluteAssetsPath = m_Project.ProjectPath() / config.AssetDirectory;
		PopulateHandleFromPath(absoluteAssetsPath);

		m_FileWatcher.removeWatch(m_WatchID);
		m_WatchID = m_FileWatcher.addWatch(absoluteAssetsPath.string(), &m_AssetsListener, true);
		m_FileWatcher.watch();
	}

	void AssetsManager::PopulateHandleFromResource()
	{
		PopulateHandleFromPath("resources");
	}

	std::filesystem::path AssetsManager::RelativePathFromHandle(const AssetHandle& handle) const
	{
		auto path = AbsolutePathFromHandle(handle);
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

	AssetHandle AssetsManager::AddEntry(const std::filesystem::path& absolutePath)
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
		auto handle = Serializer<AssetHandle>::Deserialize(path);
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
		auto it = m_Assets.find(handle);
		if (it == m_Assets.end())
			return;

		auto& asset = it->second;
		auto assetPtr = std::static_pointer_cast<Asset>(asset.Resource);
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
		for (auto& [handle, asset] : m_Assets) {
			if (asset.Dirty) {
				auto assetPtr = std::static_pointer_cast<Asset>(asset.Resource);
				assetPtr->Reload();
				asset.Dirty = false;
			}
		}
	}

	void AssetsManager::AssetsListener::handleFileAction(efsw::WatchID watchId, const std::string& dir,
														 const std::string& filename, efsw::Action action,
														 std::string oldFilename)
	{
		std::lock_guard<std::mutex> lockGuard(m_AssetsManager.m_WatcherMutex);

		FileUpdateInfo info = m_Queue.emplace();

		info.Action = action;

		info.NewPath = dir;
		info.NewPath /= filename;

		if (!oldFilename.empty()) {
			info.OldPath = dir;
			info.OldPath /= oldFilename;
		}
	}

	void AssetsManager::AssetsListener::WatcherRoutine() {
		std::lock_guard<std::mutex> lockGuard(m_AssetsManager.m_WatcherMutex);

		while (!m_Queue.empty()) {
			auto& info = m_Queue.front();
			ComputeFileInfo(info);
			m_Queue.pop();
		}

		m_AssetsManager.ReloadAllDirty();
	}

	void AssetsManager::AssetsListener::ComputeFileInfo(AssetsManager::FileUpdateInfo &updateInfo) {
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

	void AssetsManager::AssetsListener::OnFileAdded(const std::filesystem::path &newPath)
	{
		if (newPath.extension() == ".imp")
		{
			m_AssetsManager.LoadHandleFromPath(newPath);
			return;
		}

		auto impPath = newPath.string() + ".imp";

		if (std::filesystem::exists(impPath))
			return;

		m_AssetsManager.RegisterNewHandle(newPath);
	}

	void AssetsManager::AssetsListener::OnFileRemoved(const std::filesystem::path &newPath)
	{

	}

	void AssetsManager::AssetsListener::OnFileUpdate(const std::filesystem::path &newPath) {

	}

	void AssetsManager::AssetsListener::OnFileRename(const std::filesystem::path &oldPath,
													 const std::filesystem::path &newPath) {

	}
}
