#pragma once
#include <pybind11/pybind11.h>

namespace py = pybind11;
namespace Cardia
{
	enum class ScriptFieldType
	{
		Int, Float, String, List, Dict, PyBehavior, Vector2, Vector3, Vector4, Unserializable
	};

	ScriptFieldType PyHandleToFieldType(const py::handle& handle);

	class ScriptInstance
	{
	public:
		ScriptInstance(py::object instance, bool registerCallback = false);
		py::object GetAttrOrMethod(const char* name) const;
		void SetAttr(const char* name, const py::handle& value);
		ScriptFieldType GetType();
		py::object& object() { return m_Instance; }

		operator py::handle() const
		{
			return m_Instance;
		}

		operator py::object() const
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