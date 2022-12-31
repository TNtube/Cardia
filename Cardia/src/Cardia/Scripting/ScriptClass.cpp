#include "cdpch.hpp"
#include "Cardia/Scripting/ScriptClass.hpp"
#include "Cardia/Scripting/ScriptEngine.hpp"

namespace Cardia
{

	py::object DefaultObjectFromScriptFieldType(ScriptFieldType type) {
		switch (type)
		{
			case ScriptFieldType::Int: 		return py::int_();
			case ScriptFieldType::Float: 		return py::float_();
			case ScriptFieldType::String: 		return py::str();
			case ScriptFieldType::List: 		return py::list();
			case ScriptFieldType::Dict: 		return py::dict();
			case ScriptFieldType::PyBehavior: 	return py::str();
			case ScriptFieldType::Vector2:		return py::cast(glm::vec2());
			case ScriptFieldType::Vector3: 		return py::cast(glm::vec3(), py::return_value_policy::reference);
			case ScriptFieldType::Vector4: 		return py::cast(glm::vec4());
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
		py::setattr(pyInstance, "id", py::str(uuid));
		py::setattr(pyInstance, "_cd__name", py::cast(name));

		return {pyInstance, true};
	}
}