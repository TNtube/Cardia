#pragma once
#include "Cardia/Project/Project.hpp"

namespace Cardia::Serialization
{

	class ProjectSerializer {
	public:
		explicit ProjectSerializer(Project& project)
			: m_Project(project) {}

		bool Serialize(const std::filesystem::path& path);
		bool Deserialize(const std::filesystem::path& path);

	private:
		Project& m_Project;
	};
}
