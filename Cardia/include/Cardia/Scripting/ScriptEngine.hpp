#pragma once

#include <Cardia/ECS/Scene.hpp>
#include <utility>
#include <pybind11/pybind11.h>
#include "Cardia/Scripting/ScriptClass.hpp"

namespace py = pybind11;

namespace Cardia
{

	class ScriptEngine
	{
	public:
		ScriptEngine();

		ScriptEngine(const ScriptEngine& other) = delete;
		ScriptEngine(ScriptEngine&& other) noexcept = delete;
		ScriptEngine& operator=(const ScriptEngine& other) = delete;
		ScriptEngine& operator=(ScriptEngine&& other) = delete;
		~ScriptEngine();

		void OnRuntimeStart(Scene* context);
		void OnRuntimeEnd();
		void OnRuntimeUpdate();
		Scene& GetSceneContext() const;
		static ScriptEngine& Instance() { return *s_Instance; }
		static void InvalidateProject();

	private:
		void UpdateAsyncTasks();
		static ScriptEngine* s_Instance;
		Scene* m_CurrentContext;

		friend class ScriptInstance;
	};
}
