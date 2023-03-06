#pragma once

#include <filesystem>
#include <Cardia/Core/Core.hpp>

namespace Cardia
{
	struct ProjectConfig
	{
		std::string Name = "Untitled";
		std::filesystem::path StartScene;
		std::filesystem::path AssetDirectory;
	};

	class Project
	{
	public:
		ProjectConfig& GetConfig() { return m_Config; }

		static const std::filesystem::path& GetProjectDirectory()
		{
			cdCoreAssert(s_ActiveProject, "Should not be null");
			return s_ActiveProject->m_ProjectDirectory;
		}

		static std::filesystem::path GetAssetDirectory()
		{
			cdCoreAssert(s_ActiveProject, "Should not be null");
			return GetProjectDirectory() / s_ActiveProject->m_Config.AssetDirectory;
		}

		static std::shared_ptr<Project> GetActive() { return s_ActiveProject; }

		static std::shared_ptr<Project> New();
		static std::shared_ptr<Project> Load(const std::filesystem::path& path);
		static bool SaveActive(const std::filesystem::path& path);
	private:

		ProjectConfig m_Config;
		std::filesystem::path m_ProjectDirectory;
		static std::shared_ptr<Project> s_ActiveProject;
	};
}