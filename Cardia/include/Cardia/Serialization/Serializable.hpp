#pragma once
#include <json/value.h>

#include "Cardia/Application.hpp"

namespace Cardia
{
	template<typename T>
	concept Serializable = requires(T t, const Json::Value& root)
	{
		{ t.Serialize() } -> std::same_as<Json::Value>;
		{ T::Deserialize(root) } -> std::same_as<std::optional<T>>;
	};
}
