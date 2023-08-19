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

		bool HasBehavior() const { return IsSubclass<Behavior>(m_BehaviorClassDef); }
		Behavior* InstantiateBehavior(Entity entity);
		Behavior* GetBehavior() { return m_BehaviorPtr; }

		std::vector<ScriptField>& Attributes() { return m_Attributes; }
	private:
		void RetrieveScriptInfos();

		std::string m_Filename;
		py::object m_Ast;
		py::dict m_Locals;

		py::type m_BehaviorClassDef;
		py::object m_BehaviorInstance;
		Behavior* m_BehaviorPtr = nullptr;

		std::vector<ScriptField> m_Attributes;
	};
}
