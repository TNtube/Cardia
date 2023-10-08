#include "cdpch.hpp"
#include "Cardia/Scripting/ScriptEngine.hpp"
#include <pybind11/embed.h>
#include <Cardia/ECS/Components.hpp>
#include <Cardia/Project/Project.hpp>
#include "Cardia/Scripting/EmbeddedPythonModule.hpp"

namespace Cardia
{
	ScriptEngine* ScriptEngine::s_Instance = nullptr;

	ScriptEngine::ScriptEngine() : m_CurrentContext(nullptr)
	{
		CdCoreAssert(!s_Instance, "Only one m_Instance of ScriptEngine is allowed");
		s_Instance = this;

		py::initialize_interpreter();
		// m_PythonBuiltins = py::module::import("builtins");
		// m_CardiaPythonAPI = py::module::import("cardia");
	}

	ScriptEngine::~ScriptEngine()
	{
		s_Instance = nullptr;
	}

	void ScriptEngine::OnRuntimeStart(Scene* context)
	{
		m_CurrentContext = context;

		auto cardia = py::module::import("cardia");
		auto eventLoop = cardia.attr("coroutines").attr("CustomEventLoop")(true);

		auto asyncio = py::module::import("asyncio");
		asyncio.attr("set_event_loop")(eventLoop);

		const auto view = context->GetRegistry().view<Component::Script>();

		// first pass, instantiate all behaviors
		for (const auto entity : view)
		{
			auto script = view.get<Component::Script>(entity);
			if (script.IsLoaded()) {
				script.GetFile().InstantiateBehavior(Entity{ entity, context });
			}
		}
		// second pass, pass behavior references to behaviors
		for (const auto entity : view)
		{
			auto script = view.get<Component::Script>(entity);
			if (script.IsLoaded()) {
				script.GetFile().ResolveBehaviorReferences(*context);
			}
		}

		// third pass, call on_create on all behaviors
		for (const auto entity : view)
		{
			auto script = view.get<Component::Script>(entity);
			if (script.IsLoaded()) {
				try {
					auto behavior = script.GetFile().GetBehavior();
					if (behavior)
						behavior->on_create();
				} catch (const std::exception& e) {
					Log::Error("On Create : {0}", e.what());
				}
			}
		}
	}

	void ScriptEngine::OnRuntimeEnd() {
		const auto view = m_CurrentContext->GetRegistry().view<Component::Script>();
		for (const auto entity : view)
		{
			auto script = view.get<Component::Script>(entity);
			if (script.IsLoaded()) {
				try {
					script.GetFile().DestroyBehavior();
				} catch (const std::exception& e) {
					Log::Error("On Destroy : {0}", e.what());
				}
			}
		}
		m_CurrentContext = nullptr;
	}

	Scene& ScriptEngine::GetSceneContext() const
	{
		CdCoreAssert(m_CurrentContext, "No Scene Context");
		return *m_CurrentContext;
	}

	void ScriptEngine::OnRuntimeUpdate()
	{
		const auto view = m_CurrentContext->GetRegistry().view<Component::Transform, Component::Script>();
		for (const auto entity : view)
		{
			auto [transform, script] = view.get<Component::Transform, Component::Script>(entity);
			if (script.IsLoaded()) {
				try {
					auto behavior = script.GetFile().GetBehavior();
					if (behavior) {
						behavior->on_update();
					}

					if (transform.IsDirty())
						transform.RecomputeWorld({entity, m_CurrentContext});
				} catch (const std::exception& e) {
					Log::Error("On Update : {0}", e.what());
				}
			}
		}
		UpdateAsyncTasks();
	}

	void ScriptEngine::InvalidateProject()
	{
		auto sys = py::module::import("sys");
		sys.attr("path").attr("append")(Project::GetAssetDirectory().string());
	}

	void ScriptEngine::UpdateAsyncTasks()
	{
		try {
			auto asyncio = py::module::import("asyncio");
			auto loop = asyncio.attr("get_event_loop")();
			loop.attr("run_all_once")();
		} catch (const std::exception& e) {
			Log::Error("Update Async Tasks : {0}", e.what());
		}
	}


}
