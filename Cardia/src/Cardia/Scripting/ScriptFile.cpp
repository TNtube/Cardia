#include "cdpch.hpp"

#include "Cardia/Scripting/ScriptFile.hpp"
#include "Cardia/Asset/AssetsManager.hpp"

#include <pybind11/pybind11.h>
#include <Cardia/Scripting/EntityBehavior.hpp>
#include "Cardia/ECS/Scene.hpp"
#include "Cardia/ECS/Component/Script.hpp"

namespace Cardia
{
	namespace
	{
		py::object ResolveAstNode(const py::object& node, const py::module& ast, const py::dict& sourceLocals)
		{
			const auto& constant = ast.attr("Constant");
			if (py::isinstance(node, constant))
			{
				return node.attr("value");
			}

			const auto& call = ast.attr("Call");
			if (py::isinstance(node, call))
			{
				try {
					auto rValue = ast.attr("unparse")(node);
					auto result = py::eval(rValue, sourceLocals, sourceLocals);
					return result;
				} catch (const std::exception& e) {
					return py::none();
				}
			}

			const auto& attribute = ast.attr("Attribute");
			if (py::isinstance(node, attribute))
			{
				auto value = ResolveAstNode(node.attr("value"), ast, sourceLocals);
				if (value.is_none())
					return value;
				return value.attr(node.attr("node"));
			}
			const auto& name = ast.attr("Name");
			if (py::isinstance(node, name))
			{
				auto id = node.attr("id");
				if (sourceLocals.contains(id))
					return sourceLocals[id];
				if (sourceLocals["__builtins__"].contains(id))
					return sourceLocals["__builtins__"][id];
				return py::none();
			}
			return py::none();
		}
	}

	void ScriptFile::RetrieveScriptInfos()
	{
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

			// skip private attributes
			if (attrName.starts_with("_"))
				continue;

			// skip already added attributes
			if (HasScriptField(attrName))
				continue;

			const py::object annotation = ResolveAstNode(initNode.attr("annotation"), ast, m_Locals);

			ScriptField attr(attrName);
			attr.DeduceType(annotation, true, true);

			if (!attr.IsEditable())
				continue;

			const py::object attrValue = ResolveAstNode(initNode.attr("value"), ast, m_Locals);
			if (!attrValue.is_none())
				attr.SetValue(attrValue);
			m_Attributes.push_back(attr);
		}
	}

	ScriptFile::ScriptFile(std::string path, py::object ast, py::dict locals)
		: m_Filename(std::move(path)), m_Ast(std::move(ast)), m_Locals(std::move(locals)), m_BehaviorClassDef(py::type::of(py::none()))
	{
		RetrieveScriptInfos();
	}

	std::shared_ptr<ScriptFile> ScriptFile::FromSource(const std::string& source, const std::string& filename)
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

			auto code = compile(sourceAst, filename, "exec");
			// use locals as globals to retrieve builtins
			exec(code, sourceLocals, sourceLocals);

			auto file = std::make_shared<ScriptFile>(filename, sourceAst, sourceLocals);
			if (!file->HasBehavior())
			{
				Log::Error("No behavior found in script file: {0}\nPlease fix.", filename);
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

	Behavior* ScriptFile::InstantiateBehavior(Entity entity)
	{
		if (!HasBehavior())
			return nullptr;

		try {
			m_BehaviorInstance = m_BehaviorClassDef();
			m_BehaviorPtr = m_BehaviorInstance.cast<Behavior*>();
			m_BehaviorPtr->entity = entity;

			for (auto& field : m_Attributes)
			{
				if (field.GetType() != ScriptFieldType::PyBehavior) {
					py::setattr(m_BehaviorInstance, field.GetName().c_str(), field.GetValue());
				}
			}

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

	ScriptField* ScriptFile::GetScriptField(const std::string &name)
	{
		for (auto& attribute : m_Attributes)
		{
			if (attribute.GetName() == name)
				return &attribute;
		}
		return nullptr;
	}

	void ScriptFile::SetScriptField(const std::string &name, const ScriptField &field)
	{
		for (auto& attribute : m_Attributes)
		{
			if (attribute.GetName() == name)
			{
				attribute = field;
				return;
			}
		}
	}

	bool ScriptFile::HasScriptField(const std::string &name)
	{
		return std::ranges::any_of(m_Attributes, [&name](const auto& attribute) { return attribute.GetName() == name; });
	}

	void ScriptFile::ResolveBehaviorReferences(Scene &scene)
	{
		if (!HasBehavior())
			return;

		for (auto& field : m_Attributes)
		{
			if (field.GetType() != ScriptFieldType::PyBehavior)
				continue;

			try {
				auto refEntity = scene.GetEntityByUUID(field.GetValue<Component::ID>().Uuid);
				if (refEntity.IsValid() && refEntity.HasComponent<Component::Script>())
				{
					auto& refScript = refEntity.GetComponent<Component::Script>();
					auto* refBehavior = refScript.GetFile().GetBehavior();
					if (refBehavior)
						py::setattr(m_BehaviorInstance, field.GetName().c_str(), py::cast(refBehavior));
				}
			} catch (const std::exception& e) {
				py::setattr(m_BehaviorInstance, field.GetName().c_str(), py::none());
			}
		}
	}

	std::optional<Component::ID> ScriptFile::GetBehaviorAttribute(const std::string &name)
	{
		if (!HasBehavior())
			return std::nullopt;

		if (!HasScriptField(name))
			return std::nullopt;


		if (m_BehaviorPtr && !py::hasattr(m_BehaviorInstance, name.c_str()))
		{
			auto value = m_BehaviorInstance.attr(name.c_str());
			auto* behaviorRef = value.cast<Behavior*>();
			if (behaviorRef)
				return behaviorRef->entity.GetComponent<Component::ID>();
		}

		return GetScriptField(name)->GetValue<Component::ID>();
	}

	void ScriptFile::SetBehaviorAttribute(const std::string &name, Entity entity)
	{
		if (!HasBehavior())
			return;


		auto& script = entity.GetComponent<Component::Script>();

		if (m_BehaviorPtr)
		{
			py::setattr(m_BehaviorInstance, name.c_str(), script.GetFile().m_BehaviorInstance);
			return;
		}

		GetScriptField(name)->SetValue(py::cast(entity.GetComponent<Component::ID>()), false);

	}
}
