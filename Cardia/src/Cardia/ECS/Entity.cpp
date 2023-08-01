#include "cdpch.hpp"

#include "Cardia/ECS/Entity.hpp"
#include "Cardia/ECS/Components.hpp"

namespace Cardia
{
	Entity Entity::GetParent() const
	{
		return {m_Scene->m_Registry.get<Component::Relationship>(m_Entity).Parent, m_Scene};
	}

	void Entity::PopulateEntity(entt::entity parent)
	{
		if (!m_Scene || !m_Scene->m_Registry.valid(m_Entity))
			return;
		AddComponent<Component::Transform>();
		AddComponent<Component::ID>();
		AddComponent<Component::Label>(parent);
		auto& relationship = AddComponent<Component::Relationship>();

		auto& registry = m_Scene->m_Registry;

		relationship.Parent = parent;

		// TODO : test this

		auto& parentRelationship = registry.get<Component::Relationship>(parent);
		auto current = parentRelationship.FirstChild;

		if (current == entt::null)
		{
			parentRelationship.FirstChild = m_Entity;
		}
		else
		{
			while (registry.get<Component::Relationship>(current).NextSibling != entt::null)
			{
				current = registry.get<Component::Relationship>(current).NextSibling;
			}
			registry.get<Component::Relationship>(current).NextSibling = m_Entity;
			relationship.PreviousSibling = current;
		}
		
	}
}
