#include "cdpch.hpp"
#include "Cardia/Scripting/ScriptEngine.hpp"
#include <pybind11/embed.h>
#include <Cardia/ECS/Components.hpp>
#include "Cardia/Scripting/EmbeddedPythonModule.hpp"

namespace Cardia
{
	namespace py = pybind11;


	ScriptInstance::ScriptInstance(py::object instance): m_Instance(std::move(instance))
	{
		for (auto& unRegisteredCallback : ScriptEngine::Instance().m_EventMethods) {
			if (py::isinstance(m_Instance, unRegisteredCallback.first)) {
				m_OnUpdateCallbacks.insert(m_OnUpdateCallbacks.begin(),
							   unRegisteredCallback.second.begin(),
							   unRegisteredCallback.second.end());
			}
		}
	}

	py::object ScriptInstance::GetMethod(const char* name)
	{
		return m_Instance.attr(name);
	}

	ScriptClass::ScriptClass(py::object cls) : m_PyClass(std::move(cls))
	{

	}

	ScriptInstance ScriptClass::Instantiate(const UUID& uuid)
	{
		py::object pyInstance = m_PyClass();
		py::setattr(pyInstance, "id", py::str(uuid));

		return ScriptInstance(pyInstance);
	}

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
			auto instance = script.scriptClass.Instantiate(uuid.uuid);
			instance.GetMethod("on_create")();
			m_BehaviorInstances.insert({uuid.uuid, instance});
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
					instance.GetMethod(callback.c_str())();
				}
				instance.GetMethod("on_update")();
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

		if (!locals.contains(name.c_str()) || !m_PythonBuiltins.attr("issubclass")(locals[name.c_str()], m_CardiaPythonAPI.attr("Behavior")))
		{
			Log::coreError("Cannot find {0} class child of Behavior", name);
		}

		return ScriptClass(locals[name.c_str()]);
	}

}
