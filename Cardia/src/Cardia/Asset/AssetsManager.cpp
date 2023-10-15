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
				auto handle = Serializer<AssetHandle>::Deserialize(path);
				if (handle) {
					alreadyVisited.insert(path);
					m_AssetPaths[path.replace_extension()] = *handle;
				}
				continue;
			}

			const auto& impPath = std::filesystem::absolute(entry.path().string() + ".imp");

			if (alreadyVisited.contains(impPath) || std::filesystem::exists(impPath))
				continue;

			AssetHandle handle;
			m_AssetPaths[std::filesystem::absolute(path)] = handle;

			Serializer<AssetHandle> serializer(handle);
			serializer.Serialize(impPath);
		}
	}

	void AssetsManager::AssetsListener::handleFileAction(efsw::WatchID watchId, const std::string& dir,
														 const std::string& filename, efsw::Action action,
														 std::string oldFilename)
	{
		switch (action)
		{
			case efsw::Actions::Add: OnFileAdded(watchId, dir, filename);
				break;
			case efsw::Actions::Delete: OnFileRemoved(watchId, dir, filename);
				break;
			case efsw::Actions::Modified: OnFileUpdate(watchId, dir, filename);
				break;
			case efsw::Actions::Moved: OnFileRename(watchId, dir, oldFilename, filename);
				break;
		}
	}

	void AssetsManager::AssetsListener::OnFileAdded(efsw::WatchID watchId, const std::string& dir, const std::string& filename)
	{
		Log::CoreInfo("File added : {}", filename);
	}

	void AssetsManager::AssetsListener::OnFileRemoved(efsw::WatchID watchId, const std::string& dir, const std::string& filename)
	{
		Log::CoreInfo("File removed : {}", filename);
	}

	void AssetsManager::AssetsListener::OnFileUpdate(efsw::WatchID watchId, const std::string& dir, const std::string& filename)
	{
		Log::CoreInfo("File updated : {}", filename);
	}

	void AssetsManager::AssetsListener::OnFileRename(efsw::WatchID watchId, const std::string &dir,
													 const std::string &oldFilename, const std::string &newFilename)
	{
		Log::CoreInfo("File renamed from {} to {}", oldFilename, newFilename);
	}


}
