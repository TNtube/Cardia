#pragma once

#include <filesystem>
#include <Cardia/Core/Core.hpp>
#include <json/value.h>

#include "Cardia/Serialization/Serializable.hpp"

namespace Cardia
{
	struct ProjectConfig
	{
		std::string Name = "Untitled";
		std::filesystem::path StartScene;
		std::filesystem::path AssetDirectory;

		constexpr static auto properties = std::make_tuple(
			property(&ProjectConfig::Name, "Name")
			// property(&ProjectConfig::StartScene, "StartScene"),
			// property(&ProjectConfig::AssetDirectory, "AssetDirectory")
		);
	};

	class Project
	{
	public:
		const ProjectConfig& GetConfig() const { return m_Config; }
		const std::filesystem::path& ProjectPath() const { return m_ProjectDirectory; }

		static const std::filesystem::path& GetProjectDirectory()
		{
			CdCoreAssert(s_ActiveProject, "Should not be null");
			return s_ActiveProject->m_ProjectDirectory;
		}

		static const std::filesystem::path& GetAssetDirectory()
		{
			CdCoreAssert(s_ActiveProject, "Should not be null");
			return s_ActiveProject->m_AssetDirectory;
		}

		static std::shared_ptr<Project> GetActive() { return s_ActiveProject; }

		static std::shared_ptr<Project> New();
		static std::shared_ptr<Project> Load(const std::filesystem::path& path);
		static bool SaveActive(const std::filesystem::path& path);
	private:

		ProjectConfig m_Config;
		std::filesystem::path m_ProjectDirectory;
		std::filesystem::path m_AssetDirectory;
		static std::shared_ptr<Project> s_ActiveProject;

	public:
		constexpr static auto properties = std::make_tuple(
			property(&Project::m_Config, "Config")
			// property(&Project::m_ProjectDirectory, "ProjectDirectory"),
			// property(&Project::m_AssetDirectory, "AssetDirectory")
		);
	};
}
