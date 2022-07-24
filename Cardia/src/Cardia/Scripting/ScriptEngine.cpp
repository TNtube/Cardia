#include "cdpch.hpp"
#include "Cardia/Scripting/ScriptEngine.hpp"
#include <pybind11/embed.h>

namespace Cardia
{
	namespace py = pybind11;

	void ScriptEngine::init() {
		py::initialize_interpreter();
	}

	void ScriptEngine::shutdown() {
		py::finalize_interpreter();
	}
}
