#include "cdpch.hpp"
#include "Cardia/Scripting/ScriptInstance.hpp"

#include <utility>

#include "Cardia/Math/Vector2.hpp"
#include "Cardia/Scripting/ScriptEngine.hpp"

namespace Cardia
{

	namespace py = pybind11;

	ScriptInstance::ScriptInstance(py::object instance, bool registerCallback): m_Instance(std::move(instance))
	{
		// if (!registerCallback) return;
		// for (auto& unRegisteredCallback : ScriptEngine::Instance().m_EventMethods) {
		// 	if (py::isinstance(m_Instance, unRegisteredCallback.first)) {
		// 		m_OnUpdateCallbacks.insert(m_OnUpdateCallbacks.begin(),
		// 					   unRegisteredCallback.second.begin(),
		// 					   unRegisteredCallback.second.end());
		// 	}
		// }
	}

	py::object ScriptInstance::GetAttrOrMethod(const char* name) const
	{
		return m_Instance.attr(name);
	}

	void ScriptInstance::SetAttr(const char *name, const py::handle &value)
	{
		py::setattr(m_Instance, name, value);
	}
}