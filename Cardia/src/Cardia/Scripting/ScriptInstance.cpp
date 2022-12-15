#include "cdpch.hpp"
#include "Cardia/Scripting/ScriptInstance.hpp"

#include <utility>
#include "Cardia/Scripting/ScriptEngine.hpp"

namespace Cardia
{

	namespace py = pybind11;

	ScriptInstance::ScriptInstance(py::object instance): m_Instance(std::move(instance))
	{
		for (auto& unRegisteredCallback : ScriptEngine::Instance().m_EventMethods) {
			if (py::isinstance(m_Instance, unRegisteredCallback.first)) {
				m_OnUpdateCallbacks.insert(m_OnUpdateCallbacks.begin(),
							   unRegisteredCallback.second.begin(),
							   unRegisteredCallback.second.end());
			}
		}
	}

	py::object ScriptInstance::GetAttrOrMethod(const char* name)
	{
		return m_Instance.attr(name);
	}

	void ScriptInstance::SetAttr(const char *name, const py::handle &value)
	{
		py::setattr(m_Instance, name, value);
	}

	ScriptFieldType ScriptInstance::GetType() {
		return PyHandleToFieldType(m_Instance.get_type());
	}


	ScriptFieldType PyHandleToFieldType(const py::handle &handle) {
		auto pyInt   = py::int_().get_type();
		auto pyFloat = py::float_().get_type();
		auto pyStr   = py::str().get_type();
		auto pyList  = py::list().get_type();
		auto pyDict  = py::dict().get_type();
		auto pyVec2  = py::cast(glm::vec2()).get_type();
		auto pyVec3  = py::cast(glm::vec3()).get_type();
		auto pyVec4  = py::cast(glm::vec4()).get_type();

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
		if (ScriptEngine::Instance().IsSubClass(handle, pyVec2))
		{
			return ScriptFieldType::Vector2;
		}
		if (ScriptEngine::Instance().IsSubClass(handle, pyVec3))
		{
			return ScriptFieldType::Vector3;
		}
		if (ScriptEngine::Instance().IsSubClass(handle, pyVec4))
		{
			return ScriptFieldType::Vector4;
		}
		return ScriptFieldType::PyObject;
	}
}