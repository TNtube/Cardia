#include "cdpch.hpp"

#include "Cardia/ECS/Components.hpp"

namespace Cardia::Component
{
        void Script::reloadFile()
        {
                Log::info("Loading path {0}", m_Path);
                const std::ifstream t(m_Path);
                std::stringstream buffer;
                buffer << t.rdbuf();
		auto filePath = std::filesystem::path(m_Path);
		scriptClass = ScriptEngine::Instance().GetClassFromPyFile(filePath);
		scriptClass.RegisterAttributes();
        }

	glm::vec3 Transform::Forward() const
	{
		return rotation * glm::vec3{0, 0, 1.0f};
	}

	glm::vec3 Transform::Up() const
	{
		return rotation * glm::vec3{0, 1.0f, 0};
	}

	glm::vec3 Transform::Right() const
	{
		return rotation * glm::vec3{1.0f, 0, 0};
	}
}
