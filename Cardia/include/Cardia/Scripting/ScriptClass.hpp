#pragma once
#include "ScriptInstance.hpp"
#include "Cardia/Core/UUID.hpp"
#include <map>
#include <json/json_features.h>

namespace Cardia
{
	struct ScriptField {
		ScriptField() : instance(py::str()) {}
		std::string name;
		ScriptFieldType type { ScriptFieldType::Unserializable };
		ScriptFieldType keyType { ScriptFieldType::Unserializable };
		ScriptFieldType valueType { ScriptFieldType::Unserializable };
		ScriptInstance instance;

		inline bool operator==(const ScriptField& other) const {
			return name == other.name && type == other.type && valueType == other.valueType && keyType == other.keyType;
		}

		Json::Value Serialize() const;
		static std::optional<ScriptField> Deserialize(const Json::Value& root);
	};

	py::object DefaultObjectFromScriptFieldType(ScriptFieldType type);


	class ScriptClass
	{
	public:
		explicit ScriptClass(py::object cls);

		ScriptClass(const ScriptClass &) = default;
		ScriptClass() = default;

		ScriptInstance Instantiate(const UUID &uuid, const std::string& name);
		PyObject* ptr() const { return m_PyClass.ptr(); }

		operator py::handle() const
		{
			return m_PyClass;
		}

		inline bool operator== (const ScriptClass& rhs) const { return this->m_PyClass.is(rhs.m_PyClass); }

		void RegisterAttributes();
		std::vector<ScriptField>& Attributes() { return m_Attributes; }
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
			return h(py::handle(scriptClass).ptr());
		}
	};
}