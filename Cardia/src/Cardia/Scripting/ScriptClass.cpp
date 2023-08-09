#include "cdpch.hpp"
#include "Cardia/Scripting/ScriptClass.hpp"
#include "Cardia/Scripting/ScriptEngine.hpp"

namespace Cardia
{
	ScriptClass::ScriptClass(py::object cls) : m_PyClass(std::move(cls))
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
			field.Type = type;
			field.Instance = DefaultObjectFromScriptFieldType(type);
			switch (type) {
				case ScriptFieldType::List:
				{
					auto listType = item.second.attr("__args__").begin();
					field.ValueType = PyHandleToFieldType(*listType);
					break;
				}
				case ScriptFieldType::Dict:
				{
					auto keyType = item.second.attr("__args__").begin();
					field.KeyType = PyHandleToFieldType(*keyType);
					++keyType;
					field.ValueType = PyHandleToFieldType(*keyType);
					break;
				}
				default:break;
			}

			if (field.Type != ScriptFieldType::Unserializable) {
				field.Name = item.first.cast<std::string>();
				m_Attributes.emplace_back(field);
			}
		}
	}

	ScriptInstance ScriptClass::Instantiate(const UUID& uuid, const std::string& name) const
	{
		py::object pyInstance = m_PyClass();
		py::setattr(pyInstance, "id", py::str(uuid.ToString()));
		py::setattr(pyInstance, "_cd__name", py::cast(name));

		return {pyInstance, true};
	}

	bool ScriptClass::IsSubClassOf(const ScriptClass& parentClass) const
	{
		PyObject_IsSubclass(Ptr(), parentClass.Ptr());
	}

	void ScriptClass::RegisterAttributes(std::vector<ScriptField> attributes)
	{
		m_Attributes = std::move(attributes);
	}
}
