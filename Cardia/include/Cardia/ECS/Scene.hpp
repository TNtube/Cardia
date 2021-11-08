#pragma once
#include <entt/entt.hpp>


namespace Cardia
{
	class Scene
	{
	public:
		explicit Scene(std::string name);
		entt::entity createEntity();

	private:
		std::string name;
		entt::registry m_Registry;
	};
}