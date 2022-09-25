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
					py::object instance = globals[filename.c_str()]();
					std::string id = uuid.uuid;
					builtins.attr("setattr")(instance, "id", id);

					ScriptData scriptData (instance);

					for (auto py_obj : builtins.attr("dir")(instance))
					{
						auto name = py_obj.cast<std::string>();
						auto pyClass = instance.attr("__class__");

						if (!builtins.attr("hasattr")(pyClass, name.c_str()).cast<bool>()
							|| !builtins.attr("hasattr")(pyClass.attr(name.c_str()), "__name__").cast<bool>())
						{
							continue;
						}
						if (pyClass.attr(name.c_str()).attr("__name__").cast<std::string>() == "internal_update_cardia") {
							scriptData.onUpdateCallbacks.push_back(name);
						}
					}

					instance.attr("on_create")();
					s_Data->classInstances.insert({id, scriptData});
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

	void ScriptEngine::onRuntimeUpdate(DeltaTime deltaTime)
	{
		std::for_each(s_Data->classInstances.begin(), s_Data->classInstances.end(), [&](auto& instance) {
			try {
				instance.second.self.attr("on_update")(deltaTime.seconds());
				for (auto& callback : instance.second.onUpdateCallbacks) {
					instance.second.self.attr(callback.c_str())();
				}
			}
			catch (const std::exception& e) {
				Log::error("On Update : {0}", e.what());
			}
		});
	}
}
