#include "cdpch.hpp"

#include "Cardia/ECS/Entity.hpp"
#include "Cardia/ECS/Component/Relationship.hpp"

namespace Cardia
{
	Entity Entity::GetParent() const
	{
		return {m_Scene->m_Registry.get<Component::Relationship>(m_Entity).Parent, m_Scene};
	}
}
