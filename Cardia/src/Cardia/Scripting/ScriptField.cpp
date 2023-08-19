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

	Json::Value ScriptField::Serialize() const
	{
		Json::Value out;
		out["type"] = static_cast<int>(m_Type);
		out["name"] = m_Name;

		switch (m_Type) {
		case ScriptFieldType::Int:
			out["value"] = m_PyObject.cast<int>();
			break;
		case ScriptFieldType::Float:
			out["value"] = m_PyObject.cast<float>();
			break;
		case ScriptFieldType::String:
			out["value"] = m_PyObject.cast<std::string>();
			break;
		case ScriptFieldType::Vector2:
			out["value"] = m_PyObject.cast<Vector2f>().Serialize();
			break;
		case ScriptFieldType::Vector3:
			out["value"] = m_PyObject.cast<Vector3f>().Serialize();
			break;
		case ScriptFieldType::Vector4:
			out["value"] = m_PyObject.cast<Vector4f>().Serialize();
			break;
		case ScriptFieldType::PyBehavior:
		case ScriptFieldType::Dict:
		case ScriptFieldType::List:
		case ScriptFieldType::UnEditable:
			break;
		}

		return out;
	}

	std::optional<ScriptField> ScriptField::Deserialize(const Json::Value& root)
	{
		ScriptField out(root["name"].asString());
		const auto type = static_cast<ScriptFieldType>(root["type"].asInt());

		py::object instance = py::none();

		switch (type) {
		case ScriptFieldType::Int:
		{
			instance = py::cast(root["value"].asInt());
			break;
		}
		case ScriptFieldType::Float:
		{
			instance = py::cast(root["value"].asFloat());
			break;
		}
		case ScriptFieldType::String:
		{
			instance = py::cast(root["value"].asString());
			break;
		}
		case ScriptFieldType::Vector2:
		{
			auto vec2 = *Vector2f::Deserialize(root["value"]);
			instance = py::cast(vec2);
			break;
		}
		case ScriptFieldType::Vector3:
		{
			auto vec3 = *Vector3f::Deserialize(root["value"]);
			instance = py::cast(vec3);
			break;
		}
		case ScriptFieldType::Vector4:
		{
			auto vec4 = *Vector4f::Deserialize(root["value"]);
			instance = py::cast(vec4);
			break;
		}
		case ScriptFieldType::PyBehavior:
		case ScriptFieldType::Dict:
		case ScriptFieldType::List:
		case ScriptFieldType::UnEditable:
			break;
		}

		out.m_PyObject = std::move(instance);
		return out;
	}

	void ScriptField::DeduceType(const py::handle& handle, bool fromType)
	{

		auto type = ScriptFieldType::UnEditable;

		const auto pyType = py::reinterpret_borrow<py::type>(fromType ? handle : handle.get_type());
		
		if (IsSubclass<int>(pyType))
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
		// else if (IsSubclass<EntityBehavior>(pyType))
		// 	type = ScriptFieldType::PyBehavior;

		// TODO: add support for list and dict
		/* else if (IsSubclass<std::vector<py::object>>(annotation))
		{
			attr.Type = ScriptFieldType::List;
		} else if (IsSubclass<std::unordered_map<std::string, py::object>>(annotation))
		{
			attr.Type = ScriptFieldType::Dict;
		}*/

		if (type != ScriptFieldType::UnEditable && IsNone())
			m_PyObject = pyType();

		m_Type = type;
	}
}
