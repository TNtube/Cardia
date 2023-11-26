#pragma once
#include "ScriptInstance.hpp"
#include "Cardia/Core/UUID.hpp"
#include <map>
#include "Cardia/Serialization/Serializable.hpp"

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

		void DeduceType(const py::handle& handle, bool fromType = true, bool setDefault = false);

		const std::string& GetName() const { return m_Name; }
		ScriptFieldType GetType() const { return m_Type; }

		void SetValue(py::object instance, bool deduce = true) {
			m_PyObject = std::move(instance);
			if (deduce)
				DeduceType(m_PyObject, false);
		}
		template <typename T>
		T GetValue() const { return m_PyObject.cast<T>(); }
		const py::object& GetValue() const { return m_PyObject; }

		bool IsEditable() const;
		bool IsNone() const;

	private:
		std::string m_Name;
		py::object m_PyObject = py::none();
		ScriptFieldType m_Type {ScriptFieldType::UnEditable};

	public:
		constexpr static auto properties = std::make_tuple(
			property(&ScriptField::m_Name, "Name"),
			property(&ScriptField::m_Type, "Type")
			// property(&ScriptField::m_PyObject, "Value") TODO: review this
		);
	};
}
