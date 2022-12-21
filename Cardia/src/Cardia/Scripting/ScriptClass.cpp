#include "cdpch.hpp"
#include "Cardia/Scripting/ScriptClass.hpp"
#include "Cardia/Scripting/ScriptEngine.hpp"

namespace Cardia
{
	ScriptClass::ScriptClass(py::object cls) : m_PyClass(std::move(cls)), m_Attributes()
	{
		py::dict annotations(m_PyClass.attr("__annotations__"));
		for (const auto &item: annotations) {
			if (item.first.cast<std::string>().starts_with('_')) {
				continue;
			}
			const auto type = PyHandleToFieldType(item.second);

			ScriptField field{};
			field.type = type;
			switch (type) {
				case ScriptFieldType::Int:
					field.instance = py::int_();
					break;
				case ScriptFieldType::Float:
					field.instance = py::float_();
					break;
				case ScriptFieldType::String:
					field.instance = py::str();
					break;
				case ScriptFieldType::List:
				{
					auto listType = item.second.attr("__args__").begin();
					field.valueType = PyHandleToFieldType(listType);
					field.instance = py::list();
					break;
				}
				case ScriptFieldType::Dict:
				{
					auto keyType = item.second.attr("__args__").begin();
					auto valueType = keyType++;
					field.keyType = PyHandleToFieldType(keyType);
					field.valueType = PyHandleToFieldType(valueType);
					field.instance = py::dict();
					break;
				}
				case ScriptFieldType::PyBehavior:
					field.instance = py::str();
					break;
				case ScriptFieldType::Vector2:
					field.instance = py::cast(glm::vec2());
					break;
				case ScriptFieldType::Vector3:
					field.instance = py::cast(glm::vec3(), py::return_value_policy::reference);
					break;
				case ScriptFieldType::Vector4:
					field.instance = py::cast(glm::vec4());
					break;
				case ScriptFieldType::Unserializable:break;
			}

			if (field.type != ScriptFieldType::Unserializable && field.valueType != ScriptFieldType::Unserializable && field.keyType != ScriptFieldType::Unserializable) {
				m_Attributes.emplace_back(item.first.cast<std::string>(), field);
			}
		}
	}

	ScriptInstance ScriptClass::Instantiate(const UUID& uuid, const std::string& name)
	{
		py::object pyInstance = m_PyClass();
		py::setattr(pyInstance, "id", py::str(uuid));
		py::setattr(pyInstance, "_cd__name", py::cast(name));

		return {pyInstance};
	}
}