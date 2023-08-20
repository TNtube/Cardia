#pragma once

#include <pybind11/pybind11.h>

#include "ScriptClass.hpp"
#include "EntityBehavior.hpp"
#include "ScriptUtils.hpp"

namespace py = pybind11;

namespace Cardia
{
	class ScriptFile
	{
	public:
		ScriptFile(std::string path, py::object ast, py::dict locals);
		static std::shared_ptr<ScriptFile> FromSource(const std::string& source, const std::string& filename = "default.py");
		static std::shared_ptr<ScriptFile> FromPath(const std::filesystem::path& relativePath);

		template<typename T>
		T GetAttribute(const std::string& name) const
		{
			if (m_BehaviorPtr)
				return m_BehaviorInstance.attr(name.c_str()).cast<T>();
			return m_Attributes.at(name).GetValue<T>();
		}

		template<typename T>
		void SetAttribute(const std::string& name, T value)
		{
			if (m_BehaviorPtr)
				m_BehaviorInstance.attr(name.c_str()) = value;
			else
				m_Attributes.at(name).SetValue(py::cast(value));
		}

		bool HasBehavior() const { return IsSubclass<Behavior>(m_BehaviorClassDef); }
		Behavior* InstantiateBehavior(Entity entity);
		Behavior* GetBehavior() { return m_BehaviorPtr; }
		void DestroyBehavior();

		std::unordered_map<std::string, ScriptField>& Attributes() { return m_Attributes; }
	private:
		void RetrieveScriptInfos();

		std::string m_Filename;
		py::object m_Ast;
		py::dict m_Locals;

		py::type m_BehaviorClassDef;
		py::object m_BehaviorInstance;
		Behavior* m_BehaviorPtr = nullptr;

		std::unordered_map<std::string, ScriptField> m_Attributes;
	};
}
