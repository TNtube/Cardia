#pragma once
#include <entt/entt.hpp>
#include <Cardia/Core/Time.hpp>


namespace Cardia
{
	class Entity;
	namespace Panel { class SceneHierarchy; }
	class Scene
	{
	public:
		explicit Scene(std::string name);
		Entity createEntity(const std::string& name = "");

		void destroyEntity(const Entity& entity);

		void onUpdate(DeltaTime deltaTime);
		inline const char* getName() { return m_Name.c_str(); }

	private:
		std::string m_Name;
		entt::registry m_Registry;
		friend class Entity;
		friend class Panel::SceneHierarchy;
	};
}