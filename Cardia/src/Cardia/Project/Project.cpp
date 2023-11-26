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
		try
		{
			const auto project = Serializer<Project>::Deserialize(path);
			const auto projPtr = std::make_shared<Project>(project);
			projPtr->m_ProjectDirectory = canonical(path.parent_path());
			projPtr->m_AssetDirectory = canonical(projPtr->m_ProjectDirectory / projPtr->m_Config.AssetDirectory);
			s_ActiveProject = projPtr;
			return s_ActiveProject;
		}
		catch (std::exception& e)
		{
			Log::Error("Failed to load project: {0}", e.what());
			return nullptr;
		}
	}

	bool Project::SaveActive(const std::filesystem::path &path)
	{

		Serializer serializer(*s_ActiveProject);
		serializer.Serialize(path);
		s_ActiveProject->m_ProjectDirectory = canonical(path.parent_path());
		s_ActiveProject->m_AssetDirectory = canonical(s_ActiveProject->m_ProjectDirectory / s_ActiveProject->m_Config.AssetDirectory);
		return true;
	}
}
