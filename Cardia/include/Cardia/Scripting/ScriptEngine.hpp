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
		ScriptClass GetClassFromPyFile(std::filesystem::path& relativePath);
		void OnRuntimeStart(Scene* context);
		void OnRuntimeEnd();
		void OnRuntimeUpdate();
		static void UpdateWorkspace();
		ScriptInstance* GetInstance(const UUID& uuid);
		Scene& GetSceneContext();
		void RegisterUpdateMethod(py::object& cls, std::string& name);
		py::object& GetPythonBuiltins() { return m_PythonBuiltins; }
		bool IsBehavior(const py::handle& scriptClass);
		static bool IsSubClass(const ScriptClass& subClass, const ScriptClass& parentClass);
		static bool IsSubClass(const py::handle& subClass, const ScriptClass& parentClass);
		static bool IsSubClass(const ScriptClass& subClass, const py::handle& parentClass);
		static bool IsSubClass(const py::handle& subClass, const py::handle& parentClass);
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
