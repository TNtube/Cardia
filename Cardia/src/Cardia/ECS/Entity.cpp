#include "cdpch.hpp"

#include "Cardia/ECS/Entity.hpp"
#include "Cardia/ECS/Component/Relationship.hpp"

namespace Cardia
{
	Entity Entity::GetParent() const
	{
		return {m_Scene->m_Registry.get<Component::Relationship>(m_Entity).Parent, m_Scene};
	}

	ChildCollection Entity::GetChildren() const
	{
		return ChildCollection(*this);
	}

	ChildCollection::ChildIterator ChildCollection::begin() const
	{
		return ChildIterator( {m_Parent.GetComponent<Component::Relationship>().FirstChild, m_Parent.m_Scene} );
	}

	ChildCollection::ChildIterator ChildCollection::end() const
	{
		return ChildIterator( {entt::null, m_Parent.m_Scene} );
	}

	ChildCollection::ChildIterator& ChildCollection::ChildIterator::operator++()
	{
		auto& relationship = m_Entity.GetComponent<Component::Relationship>();
		m_Entity = {relationship.NextSibling, m_Entity.m_Scene};

		return *this;
	}

	const ChildCollection::ChildIterator ChildCollection::ChildIterator::operator++(int)
	{
		auto& relationship = m_Entity.GetComponent<Component::Relationship>();

		return ChildIterator( {relationship.NextSibling, m_Entity.m_Scene} );
	}
}
