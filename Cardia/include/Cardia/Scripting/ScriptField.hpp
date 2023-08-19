#pragma once
#include "ScriptInstance.hpp"
#include "Cardia/Core/UUID.hpp"
#include <map>
#include <json/json_features.h>

namespace Cardia
{
	enum class ScriptFieldType
	{
		Bool, Int, Float, String, Tuple, // Immutable
		List, Dict, Vector2, Vector3, Vector4,
		PyBehavior, UnEditable
	};

	class ScriptField {
	public:
		explicit ScriptField(std::string name) : m_Name(std::move(name)) {}

		void DeduceType(const py::handle& handle, bool fromType = true);

		std::string_view GetName() const { return m_Name; }
		ScriptFieldType GetType() const { return m_Type; }

		void SetValue(py::object instance) { m_PyObject = std::move(instance); DeduceType(m_PyObject, false); }
		template <typename T>
		T GetValue() const { return m_PyObject.cast<T>(); }
		const py::object& GetValue() const { return m_PyObject; }

		bool IsEditable() const;
		bool IsNone() const;

		Json::Value Serialize() const;
		static std::optional<ScriptField> Deserialize(const Json::Value& root);

	private:
		std::string m_Name;
		py::object m_PyObject = py::none();
		ScriptFieldType m_Type {ScriptFieldType::UnEditable};
	};
}
