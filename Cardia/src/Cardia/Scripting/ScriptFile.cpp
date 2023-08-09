#include "cdpch.hpp"
#include "Cardia/Scripting/ScriptFile.hpp"

#include "Cardia/Asset/AssetsManager.hpp"

namespace Cardia
{
	ScriptFile::ScriptFile(const std::filesystem::path& relativePath)
		: m_Path(relativePath)
	{
		std::ifstream file(AssetsManager::GetAssetAbsolutePath(relativePath));
		std::stringstream buffer;
		buffer << file.rdbuf();
		std::string source((std::istreambuf_iterator(file)), std::istreambuf_iterator<char>());

		const py::module ast = py::globals().contains("ast")
					       ? py::globals()["ast"]
					       : py::module::import("ast");

		m_Ast = ast.attr("parse")(source);
	}

	ScriptClass ScriptFile::CompileToClass() const
	{
		const auto& globals = py::globals();
		const auto& compile = globals.attr("compile");
		const auto& exec = globals.attr("exec");

		const auto& cardia = globals.contains("cardia_native")
				       ? globals["cardia_native"]
				       : py::module::import("cardia_native");

		const auto fileName = m_Path.filename().string();
		const auto fileNameWithoutExtension = fileName.substr(0, fileName.find_last_of('.'));

		const auto& code = compile(m_Ast, m_Path.filename().string().c_str(), "exec");

		// local variables for this scope
		py::object locals = py::dict();

		try
		{
			exec(code, locals);
		} catch (const std::runtime_error& e)
		{
			Log::Error(e.what());
			return {};
		}


		const auto behavior = ScriptClass(cardia.attr("Behavior"));

		if (locals.contains(fileNameWithoutExtension.c_str()))
		{
			ScriptClass scriptClass(locals[fileNameWithoutExtension.c_str()]);
			if (scriptClass.IsSubClassOf(behavior))
			{
				// TODO: register attributes & methods
				return scriptClass;
			}

			Log::Error("{0} is not a subclass of Behavior", fileNameWithoutExtension);
			return {};
		}

		Log::Error("Could not find class {0} in {1}", fileNameWithoutExtension, fileName);
		return {};
	}
}
