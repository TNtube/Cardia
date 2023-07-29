#pragma once
#include <json/value.h>

#include "Cardia/Application.hpp"

namespace Cardia
{
	template<typename T>
	concept Serializable = requires(T t, const Json::Value& root, T& other)
	{
		{ t.Serialize() } -> std::same_as<Json::Value>;
		{ T::Deserialize(root, other) } -> std::same_as<bool>;
	};
}
