#pragma once

#include "Scene.hpp"
#include "Cardia/Core/Core.hpp"

#include <entt/entt.hpp>


namespace Cardia
{

	class Entity
	{
	public:
		Entity(entt::entity entity, Scene* scene, entt::entity parent = entt::null)
			: m_Entity(entity), m_Scene(scene)
		{
			PopulateEntity(parent);
		}

		Entity() = default;

		template<typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			CdCoreAssert(!HasComponent<T>(), "Entity already has component");
			T& component = m_Scene->m_Registry.emplace<T>(m_Entity, std::forward<Args>(args)...);
			return component;
		}

		Entity GetParent() const;
		entt::entity Handle() const { return m_Entity; }

		template<typename T>
		void RemoveComponent() const
		{
			m_Scene->m_Registry.remove<T>(m_Entity);
		}

		template<typename T>
		T& GetComponent()
		{
			return m_Scene->m_Registry.get<T>(m_Entity);
		}

		template<typename T>
		bool HasComponent() const
		{
			return m_Scene->m_Registry.all_of<T>(m_Entity);
		}

		inline bool operator==(const entt::entity& entity) const
		{
			return m_Entity == entity;
		}

		bool IsValid() const { return m_Entity != entt::null; }

	private:
		void PopulateEntity(entt::entity parent);
		friend class Scene;
		entt::entity m_Entity = entt::null;
		Scene* m_Scene = nullptr;
	};
}
