#include "cdpch.hpp"
#include "Cardia/Scripting/EntityBehavior.hpp"

namespace Cardia
{
	Component::Transform& Behavior::GetTransform()
	{
		return entity.GetComponent<Component::Transform>();
	}
}