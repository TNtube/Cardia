#pragma once
#include <entt/entt.hpp>
#include <Cardia/Core/Time.hpp>


namespace Cardia
{
	class Entity;
	class Scene
	{
	public:
		explicit Scene(std::string name);
		Entity createEntity(std::string name = "");

		void onUpdate(DeltaTime deltaTime);
		inline const char* getName() { return m_Name.c_str(); }

	private:
		std::string m_Name;
		entt::registry m_Registry;
		friend class Entity;
	};
}