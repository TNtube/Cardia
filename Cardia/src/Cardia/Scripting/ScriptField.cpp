#include <Cardia/Scripting/ScriptUtils.hpp>
#include "cdpch.hpp"
#include "Cardia/Scripting/EntityBehavior.hpp"
#include "Cardia/Scripting/ScriptClass.hpp"
#include "Cardia/Scripting/ScriptEngine.hpp"

namespace Cardia
{

	bool ScriptField::IsEditable() const
	{
		return m_Type != ScriptFieldType::UnEditable;
	}

	bool ScriptField::IsNone() const
	{
		return m_PyObject.is_none();
	}

	void ScriptField::DeduceType(const py::handle& handle, bool fromType, bool setDefault)
	{

		auto type = ScriptFieldType::UnEditable;

		const auto pyType = py::reinterpret_borrow<py::type>(fromType ? handle : handle.get_type());

		if (IsSubclass<bool>(pyType))
			type = ScriptFieldType::Bool;
		else if (IsSubclass<int>(pyType))
			type = ScriptFieldType::Int;
		else if (IsSubclass<float>(pyType))
			type = ScriptFieldType::Float;
		else if (IsSubclass<std::string>(pyType))
			type = ScriptFieldType::String;
		else if (IsSubclass<Vector2f>(pyType))
			type = ScriptFieldType::Vector2;
		else if (IsSubclass<Vector3f>(pyType))
			type = ScriptFieldType::Vector3;
		else if (IsSubclass<Vector4f>(pyType))
			type = ScriptFieldType::Vector4;
		else if (IsSubclass<Behavior>(pyType))
			type = ScriptFieldType::PyBehavior;

		// TODO: add support for list and dict
		/* else if (IsSubclass<std::vector<py::object>>(annotation))
		{
			attr.Type = ScriptFieldType::List;
		} else if (IsSubclass<std::unordered_map<std::string, py::object>>(annotation))
		{
			attr.Type = ScriptFieldType::Dict;
		}*/

		if (setDefault) {
			switch (type)
			{
				case ScriptFieldType::PyBehavior:
					m_PyObject = py::cast(UUID{});
					break;
				case ScriptFieldType::UnEditable:
					break;
				default:
					m_PyObject = pyType();
					break;
			}
		}

		m_Type = type;
	}
}
