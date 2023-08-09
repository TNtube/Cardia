#pragma once

#include <pybind11/pybind11.h>

#include "ScriptClass.hpp"

namespace py = pybind11;

namespace Cardia
{
	class ScriptFile
	{
	public:
		explicit ScriptFile(const std::filesystem::path& relativePath);

		ScriptClass CompileToClass() const;
	private:
		std::filesystem::path m_Path;
		py::object m_Ast;
	};
}
