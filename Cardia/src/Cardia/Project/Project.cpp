#include "cdpch.hpp"
#include "Cardia/Project/Project.hpp"

#include "Cardia/Serialization/Serializer.hpp"

namespace Cardia
{

	std::shared_ptr<Project> Project::s_ActiveProject = std::make_shared<Project>();

	std::shared_ptr<Project> Project::New()
	{
		s_ActiveProject = std::make_shared<Project>();
		return s_ActiveProject;
	}

	std::shared_ptr<Project> Project::Load(const std::filesystem::path &path)
	{

		if (const auto project = Serializer<Project>::Deserialize(path))
		{
			const auto projPtr = std::make_shared<Project>(*project);
			projPtr->m_ProjectDirectory = canonical(path.parent_path());
			projPtr->m_AssetDirectory = canonical(projPtr->m_ProjectDirectory / projPtr->m_Config.AssetDirectory);
			s_ActiveProject = projPtr;
			return s_ActiveProject;
		}

		return nullptr;
	}

	bool Project::SaveActive(const std::filesystem::path &path)
	{

		Serializer serializer(*s_ActiveProject);
		serializer.Serialize(path);
		s_ActiveProject->m_ProjectDirectory = canonical(path.parent_path());
		s_ActiveProject->m_AssetDirectory = canonical(s_ActiveProject->m_ProjectDirectory / s_ActiveProject->m_Config.AssetDirectory);
		return true;
	}

	Json::Value Project::Serialize() const
	{
		Json::Value root;
		auto& project = root["Project"];
		project["Name"] = m_Config.Name;
		project["StartScene"] = m_Config.StartScene.string();
		project["AssetsDirectory"] = m_Config.AssetDirectory.string();

		return root;
	}

	std::optional<Project> Project::Deserialize(const Json::Value& root)
	{
		if (!root.isMember("Project"))
			return std::nullopt;

		const auto& project = root["Project"];

		if (!project.isMember("Name")
			|| !project.isMember("StartScene")
			|| !project.isMember("AssetsDirectory"))
			return std::nullopt;

		ProjectConfig config;
		config.Name = project["Name"].asString();
		config.StartScene = project["StartScene"].asString();
		config.AssetDirectory = project["AssetsDirectory"].asString();

		Project out;
		out.m_Config = config;

		return out;
	}
}
