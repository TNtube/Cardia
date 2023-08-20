#include "cdpch.hpp"

#include "Cardia/Scripting/ScriptFile.hpp"
#include "Cardia/Asset/AssetsManager.hpp"

#include <pybind11/pybind11.h>
#include <Cardia/Scripting/EntityBehavior.hpp>

namespace Cardia
{
	namespace
	{
		py::object AttributeChain(const py::object& attr, const py::module& ast, const py::dict& sourceLocals)
		{
			const auto& attribute = ast.attr("Attribute");
			if (py::isinstance(attr, attribute))
			{
				return AttributeChain(attr.attr("value"), ast, sourceLocals).attr(attr.attr("attr"));
			}
			const auto& name = ast.attr("Name");
			if (py::isinstance(attr, name))
			{
				auto id = attr.attr("id");
				return sourceLocals.contains(id)
					? sourceLocals[id]
					: sourceLocals["__builtins__"][id];
			}
			return py::none();
		}
	}

	void ScriptFile::RetrieveScriptInfos()
	{
		const auto& cardia = hasattr(m_Ast, "cardia")
			? m_Ast.attr("cardia")
			: py::module::import("cardia");

		const py::module ast = py::globals().contains("ast")
			? py::globals()["ast"]
			: py::module::import("ast");

		const auto& classDef = ast.attr("ClassDef");
		const auto& functionDef = ast.attr("FunctionDef");
		const auto& attribute = ast.attr("Attribute");
		const auto& name = ast.attr("Name");
		const auto& annAssign = ast.attr("AnnAssign");

		py::object classBody = py::none();

		auto body = py::list(m_Ast.attr("body"));

		for (auto node: body)
		{
			if (!py::isinstance(node, classDef))
				continue;

			auto className = node.attr("name");
			if (!m_Locals.contains(className) && IsSubclass<Behavior>(m_Locals[className]))
				continue;

			m_BehaviorClassDef = m_Locals[className];
			classBody = node.attr("body");
		}

		if (classBody.is_none())
		{
			// maybe warn about no class found ?
			return;
		}

		py::list initBody{};
		for (const auto classNode: classBody)
		{
			if (!py::isinstance(classNode, functionDef))
				continue;

			if (classNode.attr("name").cast<std::string>() != "__init__")
				continue;

			initBody = classNode.attr("body");
			break;
		}

		if (initBody.empty())
		{
			// is ok to not have an __init__ method
			return;
		}

		for (const auto initNode: initBody)
		{
			if (!py::isinstance(initNode, annAssign))
				continue;

			auto target = initNode.attr("target");
			if (!py::isinstance(target, attribute))
				continue;

			if (!py::isinstance(target.attr("value"), name) && target.attr("value").attr("id").cast<std::string>() != "self")
				continue;

			const auto attrName = target.attr("attr").cast<std::string>();

			const py::object annotation = AttributeChain(initNode.attr("annotation"), ast, m_Locals);

			ScriptField attr(attrName);
			attr.DeduceType(annotation);

			if (!attr.IsEditable())
				continue;

			m_Attributes.insert({attrName, attr});
		}
	}

	ScriptFile::ScriptFile(std::string path, py::object ast, py::dict locals)
		: m_Filename(std::move(path)), m_Ast(std::move(ast)), m_Locals(std::move(locals)), m_BehaviorClassDef(py::type::of(py::none()))
	{
		RetrieveScriptInfos();
	}

	std::shared_ptr<ScriptFile> ScriptFile::FromSource(const std::string& source, const std::string& sourcePath)
	{

		const py::module ast = py::globals().contains("ast")
					       ? py::globals()["ast"]
					       : py::module::import("ast");

		py::object sourceAst;
		try {
			sourceAst = ast.attr("parse")(source);
		}
		catch (const std::exception& e) {
			Log::Error("Error parsing script file: {0}\nPlease fix.", e.what());
			return nullptr;
		}
		const py::module builtins = py::globals().contains("builtins")
					       ? py::globals()["builtins"]
					       : py::module::import("builtins");
		
		const auto& compile = builtins.attr("compile");

		// don't use py::exec because of the support of ast objects
		const auto& exec = builtins.attr("exec");

		try
		{
			py::dict sourceLocals;

			auto code = compile(sourceAst, sourcePath, "exec");
			// use locals as globals to retrieve builtins
			exec(code, sourceLocals, sourceLocals);

			auto file = std::make_shared<ScriptFile>(sourcePath, sourceAst, sourceLocals);
			if (!file->HasBehavior())
			{
				Log::Error("No behavior found in script file: {0}\nPlease fix.", sourcePath);
				return nullptr;
			}
			return file;
		}
		catch (const py::type_error& e)
		{
			e.set_error();
			Log::Error("Error compiling script file: {0}\nPlease fix.", e.what());
			return nullptr;
		}
	}

	std::shared_ptr<ScriptFile> ScriptFile::FromPath(const std::filesystem::path& absolutePath)
	{
		std::ifstream file(absolutePath);
		std::stringstream buffer;
		buffer << file.rdbuf();

		return FromSource(buffer.str(), absolutePath.filename().string());
	}

	Behavior *ScriptFile::InstantiateBehavior(Entity entity)
	{
		if (!HasBehavior())
			return nullptr;

		try {
			m_BehaviorInstance = m_BehaviorClassDef();
			m_BehaviorPtr = m_BehaviorInstance.cast<Behavior*>();
			m_BehaviorPtr->entity = entity;

			for (auto& [name, field] : m_Attributes)
			{
				if (field.GetType() != ScriptFieldType::PyBehavior) {
					py::setattr(m_BehaviorInstance, name.c_str(), field.GetValue());
				}
			}

			m_BehaviorPtr->on_create();

			return m_BehaviorPtr;
		} catch (const std::exception& e) {
			Log::Error("Error instantiating behavior: {0}\nPlease fix.", e.what());
			return nullptr;
		}
	}

	void ScriptFile::DestroyBehavior()
	{
		if (m_BehaviorPtr)
			m_BehaviorPtr->on_destroy();
		m_BehaviorInstance = py::none();
		m_BehaviorPtr = nullptr;
	}
}
