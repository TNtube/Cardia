#pragma once
#include "ScriptInstance.hpp"
#include "Cardia/Core/UUID.hpp"

#include "ScriptField.hpp"

namespace Cardia
{	class ScriptClass
	{
	public:
		explicit ScriptClass(py::object cls);
		ScriptClass() = default;

		ScriptInstance Instantiate(const UUID &uuid, const std::string& name) const;
		PyObject* Ptr() const { return m_PyClass.ptr(); }

		inline bool operator== (const ScriptClass& rhs) const { return this->m_PyClass.is(rhs.m_PyClass); }

		bool IsSubClassOf(const ScriptClass& parentClass) const;
		void RegisterAttributes(std::vector<ScriptField> attributes);
		const std::vector<ScriptField>& Attributes() const { return m_Attributes; }

	private:
		std::vector<ScriptField> m_Attributes;
		py::object m_PyClass;
	};
}

namespace std {
	template<>
	struct hash<Cardia::ScriptClass>
	{
		std::size_t operator()(const Cardia::ScriptClass& scriptClass) const noexcept
		{
			static auto h = std::hash<PyObject*>{};
			return h(scriptClass.Ptr());
		}
	};
}