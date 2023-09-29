#include "cdpch.hpp"

#include "Cardia/ECS/Entity.hpp"
#include "Cardia/ECS/Component/Relationship.hpp"
#include "Cardia/ECS/Components.hpp"
#include "Cardia/Serialization/Serializable.hpp"

namespace Cardia
{
	namespace
	{
		// Recursively copy the values of b into a. Both a and b must be objects.
		void MergeJson(Json::Value &dst, const Json::Value &src)
		{
			if (!dst.isObject() || !src.isObject()) return;

			for (const auto &key: src.getMemberNames())
			{
				if (dst[key].isObject())
				{
					MergeJson(dst[key], src[key]);
				}
				else
				{
					dst[key] = src[key];
				}
			}
		}
	}


	template <Serializable Cpn>
	void SerializeOneComponent(const entt::registry& src, Json::Value& root, entt::entity entity)
	{
		if (src.all_of<Cpn>(entity))
		{
			MergeJson(root, src.get<Cpn>(entity).Serialize());
		}
	}

	template <Serializable... Cpn>
	void SerializeAllComponents(ComponentGroup<Cpn...>, const entt::registry& src, Json::Value& root, entt::entity entity)
	{
		(SerializeOneComponent<Cpn>(src, root, entity), ...);
	}

	template <Serializable Cpn>
	void DeserializeAndAssignOneComponent(const Json::Value& root, entt::registry& dst, entt::entity entity)
	{
		std::optional<Cpn> cpn = Cpn::Deserialize(root);
		if (cpn.has_value())
		{
			dst.emplace_or_replace<Cpn>(entity, *cpn);
		}
	}

	template <Serializable... Cpn>
	void DeserializeAndAssignAllComponents(ComponentGroup<Cpn...>, const Json::Value& root, entt::registry& dst, entt::entity entity)
	{
		(DeserializeAndAssignOneComponent<Cpn>(root, dst, entity), ...);
	}


	Entity Entity::GetParent() const
	{
		if (IsValid())
			return {m_Scene->m_Registry.get<Component::Relationship>(m_Entity).Parent, m_Scene};
		return {entt::null, nullptr};
	}

	ChildCollection Entity::GetChildren() const
	{
		return ChildCollection(*this);
	}

	Json::Value Entity::SerializeComponents()
	{
		if (!IsValid())
			return {};
		Json::Value out(Json::objectValue);
		SerializeAllComponents(SerializableComponents{}, m_Scene->m_Registry, out, m_Entity);
		return out;
	}

	void Entity::DeserializeAndAssignComponents(const Json::Value &root, entt::registry &dst, entt::entity entity)
	{
		DeserializeAndAssignAllComponents(SerializableComponents{}, root, dst, entity);
	}

	ChildCollection::ChildIterator ChildCollection::begin() const
	{
		if (m_Parent.IsValid())
			return ChildIterator( {m_Parent.GetComponent<Component::Relationship>().FirstChild, m_Parent.m_Scene} );
		return ChildIterator( {entt::null, m_Parent.m_Scene} );
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
