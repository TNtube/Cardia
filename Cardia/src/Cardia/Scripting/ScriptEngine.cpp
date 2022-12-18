#include "cdpch.hpp"
#include "Cardia/Scripting/ScriptEngine.hpp"
#include <pybind11/embed.h>
#include <Cardia/ECS/Components.hpp>
#include "Cardia/Scripting/EmbeddedPythonModule.hpp"

namespace Cardia
{
	ScriptEngine* ScriptEngine::s_Instance = nullptr;

	ScriptEngine::ScriptEngine() : m_CurrentContext(nullptr)
	{
		cdCoreAssert(!s_Instance, "Only one m_Instance of ScriptEngine is allowed");
		s_Instance = this;
		py::initialize_interpreter();
		m_PythonBuiltins = py::module::import("builtins");
		m_CardiaPythonAPI = py::module::import("cardia");
	}

	ScriptEngine::~ScriptEngine()
	{
		s_Instance = nullptr;
	}

	void ScriptEngine::OnRuntimeStart(Scene* context)
	{
		m_CurrentContext = context;

		const auto view = context->GetRegistry().view<Component::Transform, Component::Script, Component::ID, Component::Name>();
		for (const auto entity : view)
		{
			auto [transform, script, uuid, name] = view.get<Component::Transform, Component::Script, Component::ID, Component::Name>(entity);
			try {
				auto instance = script.scriptClass.Instantiate(uuid.uuid, name.name);
				m_BehaviorInstances.insert({uuid.uuid, instance});
			}
			catch (const std::exception& e) {
				Log::error("Instantiating : {0}", e.what());
			}
		}
		for (const auto entity : view)
		{
			auto [transform, script, uuid, name] = view.get<Component::Transform, Component::Script, Component::ID, Component::Name>(entity);
			try {
				auto behavior = m_BehaviorInstances.at(uuid.uuid);
				for (const auto& item: script.scriptClass.Attributes())
				{
					if (item.second.type == ScriptFieldType::PyBehavior) {
						try {
							auto refBehavior = ScriptEngine::Instance().GetInstance(
								UUID::fromString(py::handle(item.second.instance).cast<std::string>()));
							if (refBehavior)
							{
								py::setattr(behavior, item.first.c_str(), py::handle(*refBehavior));
							}
						} catch (const std::exception& e) {
							py::setattr(behavior, item.first.c_str(), py::none());
						}
					} else {
						py::setattr(behavior, item.first.c_str(), item.second.instance);
					}
				}
				behavior.GetAttrOrMethod("on_create")();
			}
			catch (const std::exception& e) {
				Log::error("On Create : {0}", e.what());
			}
		}
	}

	void ScriptEngine::OnRuntimeEnd() {
		m_BehaviorInstances.clear();
	}

	Scene& ScriptEngine::GetSceneContext()
	{
		return *m_CurrentContext;
	}

	void ScriptEngine::OnRuntimeUpdate()
	{
		for (auto& [uuid, instance] : m_BehaviorInstances)  {
			try {
				instance.GetAttrOrMethod("on_update")();
				for (auto& callback : instance.m_OnUpdateCallbacks) {
					auto method = instance.GetAttrOrMethod(callback.c_str());
					if (m_PythonBuiltins.attr("callable")(method).cast<bool>()) {
						method();
					}
				}
			}
			catch (const std::exception& e) {
				Log::error("On Update : {0}", e.what());
			}
		}
	}

	void ScriptEngine::RegisterUpdateMethod(py::object& cls, std::string& name)
	{
		ScriptClass scriptClass(cls);
		auto callBack = m_EventMethods.find(scriptClass);
		if (callBack != m_EventMethods.end()) {
			callBack->second.push_back(name);
		} else {
			m_EventMethods.insert({scriptClass, {name}});
		}
	}

	ScriptClass ScriptEngine::GetClassFromPyFile(std::filesystem::path& relativePath)
	{
		auto fileName = relativePath.filename().replace_extension().string();
		auto path = relativePath.replace_extension();
		auto importedFile = py::module_::import(path.string().c_str());

		if (!py::hasattr(importedFile, fileName.c_str()) && !IsSubClass(importedFile.attr(fileName.c_str()), m_CardiaPythonAPI.attr("Behavior")))
		{
			Log::coreError("Cannot find {0} class child of Behavior", fileName);
		}

		return ScriptClass(importedFile.attr(fileName.c_str()));
	}

	bool ScriptEngine::IsSubClass(const ScriptClass& subClass, const ScriptClass& parentClass) {
		return PyObject_IsSubclass(subClass.ptr(), parentClass.ptr());
	}

	bool ScriptEngine::IsSubClass(const ScriptClass& subClass, const py::handle& parentClass) {
		return PyObject_IsSubclass(subClass.ptr(), parentClass.ptr());
	}

	bool ScriptEngine::IsSubClass(const py::handle& subClass, const ScriptClass &parentClass) {
		return PyObject_IsSubclass(subClass.ptr(), parentClass.ptr());
	}

	bool ScriptEngine::IsSubClass(const py::handle &subClass, const py::handle &parentClass) {
		return PyObject_IsSubclass(subClass.ptr(), parentClass.ptr());
	}

	ScriptInstance* ScriptEngine::GetInstance(const UUID &uuid) {
		auto it = m_BehaviorInstances.find(uuid);

		if (it != m_BehaviorInstances.end()) {
			return &it->second;
		}
		return nullptr;
	}

	bool ScriptEngine::IsBehavior(const py::handle &scriptClass) {
		return IsSubClass(scriptClass, m_CardiaPythonAPI.attr("Behavior"));
	}

	void ScriptEngine::UpdateWorkspace()
	{
		auto sys = py::module::import("sys");
		sys.attr("path").attr("append")(Application::projectSettings().workspace);
	}

}
