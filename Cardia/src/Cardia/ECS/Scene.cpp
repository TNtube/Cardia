#include "cdpch.hpp"
#include "Cardia/ECS/Scene.hpp"

namespace Cardia
{
	Scene::Scene(std::string name)
		: name(std::move(name))
	{

	}

	entt::entity Scene::createEntity()
	{
		return m_Registry.create();
	}
}