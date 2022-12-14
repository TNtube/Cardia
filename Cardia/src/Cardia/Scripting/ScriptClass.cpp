#include "cdpch.hpp"
#include "Cardia/Scripting/ScriptClass.hpp"
#include "Cardia/Scripting/ScriptEngine.hpp"

namespace Cardia
{
	ScriptClass::ScriptClass(py::object cls) : m_PyClass(std::move(cls)), m_Attributes()
	{
		py::dict annotations(m_PyClass.attr("__annotations__"));
		for (const auto &item: annotations) {
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
					field.instance = py::list();
					break;
				case ScriptFieldType::Dict:
					field.instance = py::dict();
					break;
				case ScriptFieldType::PyObject:
					field.instance = py::str();
					break;
			}

			m_Attributes.insert({item.first.cast<std::string>(), field});
		}
	}

	ScriptInstance ScriptClass::Instantiate(const UUID& uuid)
	{
		py::object pyInstance = m_PyClass();
		py::setattr(pyInstance, "id", py::str(uuid));

		for (const auto& item: m_Attributes)
		{
			py::setattr(pyInstance, item.first.c_str(), item.second.instance);
		}

		return {pyInstance};
	}
}