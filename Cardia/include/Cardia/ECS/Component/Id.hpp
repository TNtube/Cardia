#pragma once

#include "Cardia/Core/UUID.hpp"
#include <json/json.h>

namespace Cardia::Component
{
	struct ID
	{
		ID() : Uuid(UUID()) {}
		explicit ID(UUID uuid)
			: Uuid(uuid) {}

		UUID Uuid;

		Json::Value Serialize() const;
		static std::optional<ID> Deserialize(const Json::Value& root);
	};
}