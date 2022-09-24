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
					Log::trace("class is instantiated");
					auto instance = globals[filename.c_str()]();
					std::string id = uuid.uuid;
					builtins.attr("setattr")(instance, "id", id);

					instance.attr("start")();
					s_Data->classInstances.insert({id, instance});
				}
			} catch (const std::exception &e) {
				Log::error(e.what());
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

	}
}
