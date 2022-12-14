#pragma once
#include <pybind11/pybind11.h>

namespace py = pybind11;
namespace Cardia
{
	class ScriptInstance
	{
	public:
		ScriptInstance(py::object instance);
		py::object GetMethod(const char* name);

		operator py::handle() const
		{
			return m_Instance;
		}

	private:
		py::object m_Instance;
		std::vector<std::string> m_OnUpdateCallbacks;

		friend class ScriptClass;
		friend class ScriptEngine;
	};
}