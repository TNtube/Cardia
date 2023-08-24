#pragma once

#include "Scene.hpp"
#include "Cardia/Core/Core.hpp"

#include <entt/entt.hpp>


namespace Cardia
{

	struct ChildCollection;

	class Entity
	{
	public:
		Entity(entt::entity entity, Scene* scene)
			: m_Entity(entity), m_Scene(scene)
		{}

		Entity() = default;

		template<typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			CdCoreAssert(!HasComponent<T>(), "Entity already has component");
			T& component = m_Scene->m_Registry.emplace<T>(m_Entity, std::forward<Args>(args)...);
			return component;
		}

		Entity GetParent() const;
		ChildCollection GetChildren() const;
		entt::entity Handle() const { return m_Entity; }
		Scene* GetScene() const { return m_Scene; }

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
		T& GetComponent() const
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

		bool IsValid() const { return  m_Scene && m_Scene->GetRegistry().valid(m_Entity); }

	private:
		friend class Scene;
		friend struct ChildCollection;
		entt::entity m_Entity = entt::null;
		Scene* m_Scene = nullptr;
	};


	struct ChildCollection
	{
		struct ChildIterator
		{
			explicit ChildIterator(const Entity firstChild) : m_Entity(firstChild) {}
			Entity operator*() const { return m_Entity; }
			Entity* operator->() { return &m_Entity; }
			ChildIterator& operator++();
			const ChildIterator operator++(int);
			bool operator==(const ChildIterator& other) const { return m_Entity.Handle() == other.m_Entity.Handle(); }
			bool operator!=(const ChildIterator& other) const { return m_Entity.Handle() != other.m_Entity.Handle(); }

		private:
			Entity m_Entity;
		};

		explicit ChildCollection(const Entity parent) : m_Parent(parent) {}

		ChildIterator begin() const;
		ChildIterator end() const;

	private:
		Entity m_Parent;
	};
}
