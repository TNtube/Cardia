#pragma once

#include "Cardia/ECS/Entity.hpp"

namespace Cardia::Component
{

	struct Relationship
	{
		Relationship() = default;

		std::size_t ChildCount = 0;
		entt::entity Parent = entt::null;
		entt::entity FirstChild = entt::null;
		entt::entity PreviousSibling = entt::null;
		entt::entity NextSibling = entt::null;
	};
}