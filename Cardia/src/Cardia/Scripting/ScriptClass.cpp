#include "cdpch.hpp"
#include "Cardia/Scripting/ScriptClass.hpp"

namespace Cardia
{
	ScriptClass::ScriptClass(py::object cls) : m_PyClass(std::move(cls))
	{

	}

	ScriptInstance ScriptClass::Instantiate(const UUID& uuid)
	{
		py::object pyInstance = m_PyClass();
		py::setattr(pyInstance, "id", py::str(uuid));

		return ScriptInstance(pyInstance);
	}
}