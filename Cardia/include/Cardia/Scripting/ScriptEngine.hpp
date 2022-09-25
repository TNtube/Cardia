#pragma once

#include <Cardia/ECS/Scene.hpp>
#include <pybind11/pybind11.h>

namespace py = pybind11;

namespace Cardia
{
	struct ScriptData
	{
		py::object self;
		std::vector<std::string> onUpdateCallbacks;

		explicit ScriptData(py::object& instance) {
			self = instance;
			onUpdateCallbacks = {};
		}
	};

	struct ScriptEngineData
	{
	public:
		Scene* currentContext;
		std::unordered_map<std::string, ScriptData> classInstances;
	};


        class ScriptEngine
        {
        public:
                static void init();
		static void shutdown();
		static void onRuntimeStart(Scene* context);
		static void onRuntimeUpdate(DeltaTime deltaTime);
		static Scene* getSceneContext();
	private:
		static std::unique_ptr<ScriptEngineData> s_Data;
        };
}
