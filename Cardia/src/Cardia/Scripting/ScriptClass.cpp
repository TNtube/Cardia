#include "cdpch.hpp"
#include "Cardia/Scripting/ScriptClass.hpp"
#include "Cardia/Scripting/ScriptEngine.hpp"

namespace Cardia
{
	ScriptClass::ScriptClass(py::object cls) : m_PyClass(std::move(cls))
	{
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
		return PyObject_IsSubclass(Ptr(), parentClass.Ptr());
	}

	void ScriptClass::RegisterAttributes(std::vector<ScriptField> attributes)
	{
		m_Attributes = std::move(attributes);
	}
}
