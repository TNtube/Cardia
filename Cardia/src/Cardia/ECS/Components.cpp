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
		glm::vec3 forward;

		forward.x = cos(rotation.x) * sin(rotation.y);
		forward.y = -sin(rotation.x);
		forward.z = cos(rotation.x) * cos(rotation.y);

		return forward;
	}

	glm::vec3 Transform::Up() const
	{
		return glm::cross(Forward(), Right());
	}

	glm::vec3 Transform::Right() const
	{
		glm::vec3 right;
		right.x =  cos(rotation.y);
		right.y =  0;
		right.z = -sin(rotation.y);
		return right;
	}
}
