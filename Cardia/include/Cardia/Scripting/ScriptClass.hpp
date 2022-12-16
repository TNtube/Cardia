#pragma once
#include "ScriptInstance.hpp"
#include "Cardia/Core/UUID.hpp"

namespace Cardia
{
	struct ScriptField {
		ScriptField() : instance(py::str()) {}
		ScriptFieldType type {};
		ScriptInstance instance;
	};

	class ScriptClass
	{
	public:
		explicit ScriptClass(py::object cls);

		ScriptClass(const ScriptClass &) = default;
		ScriptClass() = default;

		ScriptInstance Instantiate(const UUID &uuid);
		PyObject* ptr() const { return m_PyClass.ptr(); }

		operator py::handle() const
		{
			return m_PyClass;
		}

		inline bool operator== (const ScriptClass& rhs) const { return this->m_PyClass.is(rhs.m_PyClass); }

		std::unordered_map<std::string, ScriptField>& Attributes() { return m_Attributes; }
		const std::unordered_map<std::string, ScriptField>& Attributes() const { return m_Attributes; }

	private:
		std::unordered_map<std::string, ScriptField> m_Attributes;
		py::object m_PyClass;
	};
}

namespace std {
	template<>
	struct hash<Cardia::ScriptClass>
	{
		std::size_t operator()(const Cardia::ScriptClass& scriptClass) const
		{
			static auto h = std::hash<PyObject*>{};
			return h(py::handle(scriptClass).ptr());
		}
	};
}