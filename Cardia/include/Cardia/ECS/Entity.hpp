#pragma once
#include <entt/entt.hpp>
#include "Scene.hpp"
#include "Cardia/Core/Core.hpp"


namespace Cardia
{

	class Entity
	{
	public:
		Entity(entt::entity entity, Scene* scene)
			: m_Entity(entity), m_Scene(scene)
		{

		}

		Entity() = default;
		Entity(const Entity&) = default;

		template<typename T, typename... Args>
		T& addComponent(Args&&... args)
		{
			cdCoreAssert(!hasComponent<T>(), "Entity {0}");
			T& component = m_Scene->m_Registry.emplace<T>(m_Entity, std::forward<Args>(args)...);
			return component;
		}

		template<typename T>
		T& getComponent()
		{
			return m_Scene->m_Registry.get<T>(m_Entity);
		}

		template<typename T>
		bool hasComponent()
		{
			return m_Scene->m_Registry.all_of<T>(m_Entity);
		}

		inline bool operator==(const entt::entity& entity) {
			return m_Entity == entity;
		}

		inline explicit operator bool() {
			return m_Entity != entt::null;
		}

	private:
		entt::entity m_Entity = entt::null;
		Scene* m_Scene = nullptr;
	};
}