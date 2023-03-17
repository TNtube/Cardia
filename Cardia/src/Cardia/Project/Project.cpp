#include "cdpch.hpp"
#include "Cardia/Project/Project.hpp"
#include "Cardia/Serialization/ProjectSerializer.hpp"

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
		auto proj = std::make_shared<Project>();

		Serialization::ProjectSerializer serializer(*proj);

		if (serializer.Deserialize(path)) {
			proj->m_ProjectDirectory = std::filesystem::canonical(path.parent_path());
			s_ActiveProject = proj;
			return s_ActiveProject;
		}

		return nullptr;
	}

	bool Project::SaveActive(const std::filesystem::path &path)
	{
		Serialization::ProjectSerializer serializer(*s_ActiveProject);
		if (serializer.Serialize(path)) {
			s_ActiveProject->m_ProjectDirectory = std::filesystem::canonical(path.parent_path());
			return true;
		}
		return false;
	}
}