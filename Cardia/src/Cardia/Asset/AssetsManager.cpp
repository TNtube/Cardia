#include "cdpch.hpp"

#include <Cardia/Project/Project.hpp>
#include <Cardia/Renderer/Renderer.hpp>
#include "Cardia/Asset/AssetsManager.hpp"

constexpr std::chrono::duration<float> GC_COLLECTION_DURATION = std::chrono::duration<float>(2.0f);
constexpr std::uint32_t MAX_UNUSED_COUNT = 2;

namespace Cardia
{
	void AssetsManager::PopulateHandleFromProject(const Project &project)
	{
		m_Project = project;
		auto& config = m_Project.GetConfig();
		auto absoluteAssetsPath = m_Project.ProjectPath() / config.AssetDirectory;
		PopulateHandleFromPath(absoluteAssetsPath);
	}

	void AssetsManager::PopulateHandleFromResource()
	{
		PopulateHandleFromPath("resources");
	}

	std::filesystem::path AssetsManager::RelativePathFromHandle(const AssetHandle &handle) const
	{
		auto path = AbsolutePathFromHandle(handle);

		if (!path.empty())
			return std::filesystem::relative(path, m_Project.ProjectPath() / m_Project.GetConfig().AssetDirectory);

		return {};
	}

	std::filesystem::path AssetsManager::AbsolutePathFromHandle(const AssetHandle &handle) const
	{
		for (const auto& [path, h] : m_AssetPaths) {
			if (h == handle)
				return path;
		}
		return {};
	}

	AssetHandle AssetsManager::AddEntry(const std::filesystem::path &absolutePath)
	{
		AssetHandle newHandle;
		m_AssetPaths[std::filesystem::absolute(absolutePath)] = newHandle;
		return newHandle;
	}

	void AssetsManager::PopulateHandleFromPath(const std::filesystem::path &abs)
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
}
