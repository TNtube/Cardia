#include "cdpch.hpp"
#include "Cardia/Scripting/ScriptClass.hpp"
#include "Cardia/Scripting/ScriptEngine.hpp"

namespace Cardia
{
	Json::Value ScriptField::Serialize() const
	{
		Json::Value out;
		out["type"] = static_cast<int>(Type);
		out["name"] = Name;

		switch (Type) {
		case ScriptFieldType::Int:
			out["value"] = py::handle(Instance).cast<int>();
			break;
		case ScriptFieldType::Float:
			out["value"] = py::handle(Instance).cast<float>();
			break;
		case ScriptFieldType::String:
			out["value"] = py::handle(Instance).cast<std::string>();
			break;
		case ScriptFieldType::List:
			{
				py::list list(Instance);
				for (int index = 0; index < py::len(list); index++) {
					ScriptField subField;
					subField.Name = std::to_string(index);
					subField.Instance = py::object(list[index]);
					switch (subField.ValueType) {
					case ScriptFieldType::List:
					case ScriptFieldType::Dict:
						subField.Type = ScriptFieldType::Unserializable;
						break;
					default:
						subField.Type = ValueType;
						break;
					}
					out["value"].append(subField.Serialize());
				}
				break;
			}
		case ScriptFieldType::Dict:break;
		case ScriptFieldType::PyBehavior:
			out["value"] = py::handle(Instance).cast<std::string>();
			break;
		case ScriptFieldType::Vector2:
			out["value"] = py::handle(Instance).cast<Vector2f>().Serialize();
			break;
		case ScriptFieldType::Vector3:
			out["value"] = py::handle(Instance).cast<Vector3f>().Serialize();
			break;
		case ScriptFieldType::Vector4:
			out["value"] = py::handle(Instance).cast<Vector4f>().Serialize();
			break;
		case ScriptFieldType::Unserializable:break;
		}

		return out;
	}

	std::optional<ScriptField> ScriptField::Deserialize(const Json::Value& root)
	{
		ScriptField out;

		out.Name = root["name"].asString();
		out.Type = static_cast<ScriptFieldType>(root["type"].asInt());

		switch (out.Type) {
		case ScriptFieldType::Int:
			out.Instance = py::cast(root["value"].asInt());
			break;
		case ScriptFieldType::Float:
			out.Instance = py::cast(root["value"].asFloat());
			break;
		case ScriptFieldType::String:
			out.Instance = py::cast(root["value"].asString());
			break;
		case ScriptFieldType::List:
			{
				const py::list list;
				for (auto& subNode: root["value"]) {
					if (auto subField = Deserialize(subNode))
					{
						out.ValueType = subField->Type;
						list.attr("append")(subField->Instance.object());
					}
				}
				out.Instance = list;
				break;
			}
		case ScriptFieldType::Dict:break;
		case ScriptFieldType::PyBehavior:
			out.Instance = py::cast(root["value"].asString());
			break;
		case ScriptFieldType::Vector2:
			auto vec2 = *Vector2f::Deserialize(root["value"]);
			out.Instance = py::cast(vec2);
			break;
		case ScriptFieldType::Vector3:
			auto vec3 = *Vector3f::Deserialize(root["value"]);
			out.Instance = py::cast(vec3);
			break;
		case ScriptFieldType::Vector4:
			auto vec4 = *Vector4f::Deserialize(root["value"]);
			out.Instance = py::cast(vec4);
			break;
		case ScriptFieldType::Unserializable:break;
		}
		return out;
	}
}