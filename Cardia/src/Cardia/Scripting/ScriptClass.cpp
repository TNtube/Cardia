#include "cdpch.hpp"
#include "Cardia/Scripting/ScriptClass.hpp"
#include "Cardia/Scripting/ScriptEngine.hpp"

namespace Cardia
{
	ScriptFieldType PyHandleToFieldType(const py::handle &handle) {
		auto pyInt   = py::int_().get_type();
		auto pyFloat = py::float_().get_type();
		auto pyStr   = py::str().get_type();
		auto pyList  = py::list().get_type();
		auto pyDict  = py::dict().get_type();

		if (pyInt.equal(handle))
		{
			return ScriptFieldType::Int;
		}
		if (pyFloat.equal(handle))
		{
			return ScriptFieldType::Float;
		}
		if (pyStr.equal(handle))
		{
			return ScriptFieldType::String;
		}
		if (pyList.equal(handle))
		{
			return ScriptFieldType::List;
		}
		if (pyDict.equal(handle))
		{
			return ScriptFieldType::Dict;
		}
		return ScriptFieldType::PyObject;
	}

	ScriptClass::ScriptClass(py::object cls) : m_PyClass(std::move(cls)), m_Attributes()
	{
		py::dict annotations(m_PyClass.attr("__annotations__"));
		for (const auto &item: annotations) {
			const auto type = PyHandleToFieldType(item.second);

			ScriptField field{};
			field.type = type;
			field.instance = py::str();

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