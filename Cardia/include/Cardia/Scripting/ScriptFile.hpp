#pragma once

#include <pybind11/pybind11.h>

#include "ScriptClass.hpp"

namespace py = pybind11;

namespace Cardia
{
	class ScriptFile
	{
	public:
		ScriptFile(std::string path, py::object ast, py::dict locals, std::vector<ScriptField> attributes);
		static std::shared_ptr<ScriptFile> FromSource(const std::string& source, const std::string& filename = "default.py");
		static std::shared_ptr<ScriptFile> FromPath(const std::filesystem::path& relativePath);

		std::vector<ScriptField>& Attributes() { return m_Attributes; }
		
		ScriptClass CompileToClass() const;
	private:

		std::string m_Filename;
		py::object m_Ast;
		py::dict m_Locals;

		std::vector<ScriptField> m_Attributes;
	};
}
