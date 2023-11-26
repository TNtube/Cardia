#pragma once

#include "Cardia/Core/UUID.hpp"
#include <json/json.h>

#include "Cardia/Serialization/Serializable.hpp"

namespace Cardia::Component
{
	struct ID
	{
		ID() : Uuid(UUID()) {}
		explicit ID(UUID uuid)
			: Uuid(std::move(uuid)) {}

		UUID Uuid;

		constexpr static auto properties = std::make_tuple(
			property(&ID::Uuid, "Uuid")
		);
	};
}
