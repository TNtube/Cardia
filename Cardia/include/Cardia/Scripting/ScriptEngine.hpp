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
		~ScriptEngine();
		ScriptClass GetClassFromPyFile(const std::string& name, const std::string& content);
		bool IsSubClass(const ScriptClass& subClass, const ScriptClass& parentClass);
		bool IsSubClass(const py::handle& subClass, const py::handle& parentClass);
		void OnRuntimeStart(Scene* context);
		void OnRuntimeEnd();
		void OnRuntimeUpdate();
		Scene& GetSceneContext();
		void RegisterUpdateMethod(py::object& cls, std::string& name);
		py::object& GetPythonBuiltins() { return m_PythonBuiltins; }
		static ScriptEngine& Instance() { return *s_Instance; }

	private:
		static ScriptEngine* s_Instance;
		Scene* m_CurrentContext;
		std::unordered_map<UUID, ScriptInstance> m_BehaviorInstances;
		std::unordered_map<ScriptClass, std::vector<std::string>> m_EventMethods;
		py::object m_PythonBuiltins;
		py::object m_CardiaPythonAPI;

		friend class ScriptInstance;
	};
}
