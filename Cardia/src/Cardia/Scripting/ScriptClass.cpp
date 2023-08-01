#include "cdpch.hpp"
#include "Cardia/Scripting/ScriptClass.hpp"
#include "Cardia/Scripting/ScriptEngine.hpp"

namespace Cardia
{
	Json::Value ScriptField::Serialize() const
	{
		Json::Value out;
		out["type"] = static_cast<int>(type);
		out["name"] = name;

		switch (type) {
		case ScriptFieldType::Int:
			out["value"] = py::handle(instance).cast<int>();
			break;
		case ScriptFieldType::Float:
			out["value"] = py::handle(instance).cast<float>();
			break;
		case ScriptFieldType::String:
			out["value"] = py::handle(instance).cast<std::string>();
			break;
		case ScriptFieldType::List:
			{
				py::list list(instance);
				for (int index = 0; index < py::len(list); index++) {
					ScriptField subField;
					subField.name = std::to_string(index);
					subField.instance = py::object(list[index]);
					switch (subField.valueType) {
					case ScriptFieldType::List:
					case ScriptFieldType::Dict:
						subField.type = ScriptFieldType::Unserializable;
						break;
					default:
						subField.type = valueType;
						break;
					}
					out["value"].append(subField.Serialize());
				}
				break;
			}
		case ScriptFieldType::Dict:break;
		case ScriptFieldType::PyBehavior:
			out["value"] = py::handle(instance).cast<std::string>();
			break;
		case ScriptFieldType::Vector2:
			out["value"] = py::handle(instance).cast<Vector2f>().Serialize();
			break;
		case ScriptFieldType::Vector3:
			out["value"] = py::handle(instance).cast<Vector3f>().Serialize();
			break;
		case ScriptFieldType::Vector4:
			out["value"] = py::handle(instance).cast<Vector4f>().Serialize();
			break;
		case ScriptFieldType::Unserializable:break;
		}

		return out;
	}

	std::optional<ScriptField> ScriptField::Deserialize(const Json::Value& root)
	{
		ScriptField out;

		out.name = root["name"].asString();
		out.type = static_cast<ScriptFieldType>(root["type"].asInt());

		switch (out.type) {
		case ScriptFieldType::Int:
			out.instance = py::cast(root["value"].asInt());
			break;
		case ScriptFieldType::Float:
			out.instance = py::cast(root["value"].asFloat());
			break;
		case ScriptFieldType::String:
			out.instance = py::cast(root["value"].asString());
			break;
		case ScriptFieldType::List:
			{
				const py::list list;
				for (auto& subNode: root["value"]) {
					if (auto subField = Deserialize(subNode))
					{
						out.valueType = subField->type;
						list.attr("append")(subField->instance.object());
					}
				}
				out.instance = list;
				break;
			}
		case ScriptFieldType::Dict:break;
		case ScriptFieldType::PyBehavior:
			out.instance = py::cast(root["value"].asString());
			break;
		case ScriptFieldType::Vector2:
			auto vec2 = *Vector2f::Deserialize(root["value"]);
			out.instance = py::cast(vec2);
			break;
		case ScriptFieldType::Vector3:
			auto vec3 = *Vector3f::Deserialize(root["value"]);
			out.instance = py::cast(vec3);
			break;
		case ScriptFieldType::Vector4:
			auto vec4 = *Vector4f::Deserialize(root["value"]);
			out.instance = py::cast(vec4);
			break;
		case ScriptFieldType::Unserializable:break;
		}
		return out;
	}

	py::object DefaultObjectFromScriptFieldType(ScriptFieldType type) {
		switch (type)
		{
			case ScriptFieldType::Int: 		return py::int_();
			case ScriptFieldType::Float: 		return py::float_();
			case ScriptFieldType::String: 		return py::str();
			case ScriptFieldType::List: 		return py::list();
			case ScriptFieldType::Dict: 		return py::dict();
			case ScriptFieldType::PyBehavior: 	return py::str();
			case ScriptFieldType::Vector2:		return py::cast(Vector2f());
			case ScriptFieldType::Vector3: 		return py::cast(Vector3f());
			case ScriptFieldType::Vector4: 		return py::cast(Vector4f());
			default:				return py::none();
		}
	}

	ScriptClass::ScriptClass(py::object cls) : m_PyClass(std::move(cls)), m_Attributes()
	{
	}

	void ScriptClass::RegisterAttributes()
	{
		py::dict annotations(m_PyClass.attr("__annotations__"));
		for (const auto &item: annotations) {
			if (item.first.cast<std::string>().starts_with('_')) {
				continue;
			}
			const auto type = PyHandleToFieldType(item.second);

			ScriptField field{};
			field.type = type;
			field.instance = DefaultObjectFromScriptFieldType(type);
			switch (type) {
				case ScriptFieldType::List:
				{
					auto listType = item.second.attr("__args__").begin();
					field.valueType = PyHandleToFieldType(*listType);
					break;
				}
				case ScriptFieldType::Dict:
				{
					auto keyType = item.second.attr("__args__").begin();
					field.keyType = PyHandleToFieldType(*keyType);
					keyType++;
					field.valueType = PyHandleToFieldType(*keyType);
					break;
				}
				default:break;
			}

			if (field.type != ScriptFieldType::Unserializable) {
				field.name = item.first.cast<std::string>();
				m_Attributes.emplace_back(field);
			}
		}
	}

	ScriptInstance ScriptClass::Instantiate(const UUID& uuid, const std::string& name)
	{
		py::object pyInstance = m_PyClass();
		py::setattr(pyInstance, "id", py::str(uuid.ToString()));
		py::setattr(pyInstance, "_cd__name", py::cast(name));

		return {pyInstance, true};
	}
}