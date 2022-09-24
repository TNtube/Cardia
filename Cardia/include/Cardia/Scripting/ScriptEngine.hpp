#pragma once

#include <Cardia/ECS/Scene.hpp>
#include <pybind11/pybind11.h>

namespace py = pybind11;

namespace Cardia
{
	struct ScriptEngineData
	{
	public:
		Scene* currentContext;
		std::unordered_map<std::string, py::object> classInstances;
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
