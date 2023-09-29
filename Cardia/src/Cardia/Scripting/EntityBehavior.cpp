#include "cdpch.hpp"
#include "Cardia/Scripting/EntityBehavior.hpp"

namespace Cardia
{
	Component::Transform& Behavior::GetTransform()
	{
		return entity.GetComponent<Component::Transform>();
	}

	Entity Behavior::Spawn(const std::string& name) const
	{
		return entity.GetScene()->CreateEntity(name, entity.Handle());
	}
}