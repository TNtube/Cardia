#include "cdpch.hpp"
#include "Cardia/Scripting/ScriptInstance.hpp"
#include "Cardia/Scripting/ScriptEngine.hpp"

namespace Cardia
{

	namespace py = pybind11;

	ScriptInstance::ScriptInstance(py::handle instance): m_Instance(instance)
	{
		for (auto& unRegisteredCallback : ScriptEngine::Instance().m_EventMethods) {
			if (py::isinstance(m_Instance, unRegisteredCallback.first)) {
				m_OnUpdateCallbacks.insert(m_OnUpdateCallbacks.begin(),
							   unRegisteredCallback.second.begin(),
							   unRegisteredCallback.second.end());
			}
		}
	}

	py::object ScriptInstance::GetMethod(const char* name)
	{
		return m_Instance.attr(name);
	}
}