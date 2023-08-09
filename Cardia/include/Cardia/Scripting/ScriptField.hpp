#pragma once
#include "ScriptInstance.hpp"
#include "Cardia/Core/UUID.hpp"
#include <map>
#include <json/json_features.h>

namespace Cardia
{
	struct ScriptField {
		ScriptField() : Instance(py::str()) {}
		std::string Name;
		ScriptFieldType Type { ScriptFieldType::Unserializable };
		ScriptFieldType KeyType { ScriptFieldType::Unserializable };
		ScriptFieldType ValueType { ScriptFieldType::Unserializable };
		ScriptInstance Instance;

		inline bool operator==(const ScriptField& other) const {
			return Name == other.Name && Type == other.Type && ValueType == other.ValueType && KeyType == other.KeyType;
		}

		Json::Value Serialize() const;
		static std::optional<ScriptField> Deserialize(const Json::Value& root);
	};
}