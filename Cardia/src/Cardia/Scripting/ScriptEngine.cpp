#include "cdpch.hpp"
#include "Cardia/Scripting/ScriptEngine.hpp"
#include <pybind11/embed.h>
#include <Cardia/ECS/Components.hpp>
#include "Cardia/Scripting/EmbeddedPythonModule.hpp"

namespace Cardia
{
	namespace py = pybind11;

	std::unique_ptr<ScriptEngineData> ScriptEngine::s_Data = nullptr;

	void ScriptEngine::init() {
		py::initialize_interpreter();
		s_Data = std::make_unique<ScriptEngineData>();
	}

	void ScriptEngine::shutdown() {
		s_Data.reset();
		py::finalize_interpreter();
	}

	void ScriptEngine::onRuntimeStart(Scene* context)
	{
		const auto viewTransform = context->getRegistry().view<Component::Script, Component::ID>();
		for (auto [entity, script, uuid] : viewTransform.each())
		{
			try {
				const auto filename = std::filesystem::path(script.getPath()).filename().replace_extension().string();
				auto builtins = py::module::import("builtins");
				auto cardiapy = py::module::import("cardia");

				auto globals = py::globals();

				py::exec(script.getContent(), globals, globals);

				if (builtins.attr("issubclass")(globals[filename.c_str()], cardiapy.attr("Behavior")))
				{
					std::string id = uuid.uuid;
					if (!s_Data->classInstances.contains(id)) {
						py::object cls = globals[filename.c_str()];
						ScriptData scriptData = instantiatePythonClass(cls, id);
						s_Data->classInstances.insert({id, scriptData});
					}
				}
			} catch (const std::exception &e) {
				Log::error("On Init : {0}", e.what());
			}
		}

		s_Data->currentContext = context;
	}

	Scene* ScriptEngine::getSceneContext()
	{
		return s_Data->currentContext;
	}

	void ScriptEngine::onRuntimeUpdate()
	{
		for (auto& function : s_Data->onUpdateFunctions) {
			try {
				function();
			} catch (const std::exception& e) {
				Log::error("On Update : {0}", e.what());
			}
		}
		for (auto& instance : s_Data->classInstances)  {
			try {
				instance.second.self.attr("on_update")();
				for (auto& callback : instance.second.onUpdateCallbacks) {
					instance.second.self.attr(callback.c_str())();
				}
			}
			catch (const std::exception& e) {
				Log::error("On Update : {0}", e.what());
			}
		};
	}

	void ScriptEngine::registerUpdateMethod(py::object& obj, std::string& name) {
		s_Data->unRegisteredCallbacks.emplace_back(obj, name);
	}

	ScriptData ScriptEngine::instantiatePythonClass(py::object& classObj, const std::string& id) {
		py::object instance = classObj();
		auto builtins = py::module::import("builtins");
		builtins.attr("setattr")(instance, "id", id);

		ScriptData scriptData (instance);
		instance.attr("on_create")();
		for (auto& unRegisteredCallback : s_Data->unRegisteredCallbacks) {
			if (py::isinstance(instance, unRegisteredCallback.first)) {
				scriptData.onUpdateCallbacks.emplace_back(unRegisteredCallback.second);
			}
		}

		return scriptData;
	}

	void ScriptEngine::registerUpdateFunction(py::object &obj)
	{
		s_Data->onUpdateFunctions.push_back(obj);
	}
}
