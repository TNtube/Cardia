#pragma once

#include "Scene.hpp"
#include "Cardia/Core/Core.hpp"

#include <entt/entt.hpp>


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
			cdCoreAssert(!hasComponent<T>(), "Entity already has component");
			T& component = m_Scene->m_Registry.emplace<T>(m_Entity, std::forward<Args>(args)...);
			return component;
		}

		template<typename T>
		void removeComponent() const
		{
			m_Scene->m_Registry.remove<T>(m_Entity);
		}

		template<typename T>
		T& getComponent()
		{
			return m_Scene->m_Registry.get<T>(m_Entity);
		}

		template<typename T>
		bool hasComponent() const
		{
			return m_Scene->m_Registry.all_of<T>(m_Entity);
		}

		inline bool operator==(const entt::entity& entity) const
		{
			return m_Entity == entity;
		}

		inline explicit operator bool() const
		{
			return m_Entity != entt::null;
		}

	private:
		friend class Scene;
		entt::entity m_Entity = entt::null;
		Scene* m_Scene = nullptr;
	};
}