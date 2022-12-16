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

		const auto view = context->GetRegistry().view<Component::Transform, Component::Script, Component::ID>();
		for (const auto entity : view)
		{
			auto [transform, script, uuid] = view.get<Component::Transform, Component::Script, Component::ID>(entity);
			try {
				auto instance = script.scriptClass.Instantiate(uuid.uuid);
				instance.GetAttrOrMethod("on_create")();
				m_BehaviorInstances.insert({uuid.uuid, instance});
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
				for (auto& callback : instance.m_OnUpdateCallbacks) {
					instance.GetAttrOrMethod(callback.c_str())();
				}
				instance.GetAttrOrMethod("on_update")();
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

	ScriptClass ScriptEngine::GetClassFromPyFile(const std::string& name, const std::string& content)
	{
		auto locals = py::dict();

		py::exec(content, locals, locals);

		if (!locals.contains(name.c_str()) || !IsSubClass(locals[name.c_str()], m_CardiaPythonAPI.attr("Behavior")))
		{
			Log::coreError("Cannot find {0} class child of Behavior", name);
		}

		return ScriptClass(locals[name.c_str()]);
	}

	bool ScriptEngine::IsSubClass(const ScriptClass &subClass, const ScriptClass &parentClass) {
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

}
