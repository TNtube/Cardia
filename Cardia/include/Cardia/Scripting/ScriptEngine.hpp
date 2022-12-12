#pragma once

#include <Cardia/ECS/Scene.hpp>
#include <utility>
#include <pybind11/pybind11.h>

namespace py = pybind11;

namespace Cardia
{
	class ScriptInstance
	{
		explicit ScriptInstance(py::object instance);
		py::object GetMethod(const char* name);

	private:
		py::object m_Instance;
		std::vector<std::string> m_OnUpdateCallbacks;

		friend class ScriptClass;
		friend class ScriptEngine;
	};

	class ScriptClass
	{
	public:
		explicit ScriptClass(py::object cls);

		ScriptClass(const ScriptClass &) = default;
		ScriptClass() = default;

		ScriptInstance Instantiate(const UUID &uuid);

		operator py::handle() const
		{
			return m_PyClass;
		}

		inline bool operator== (const ScriptClass& rhs) const { return this->m_PyClass.is(rhs.m_PyClass); }

	private:
		py::object m_PyClass;
	};
}

namespace std {
	template<>
	struct hash<Cardia::ScriptClass>
	{
		std::size_t operator()(const Cardia::ScriptClass& scriptClass) const
		{
			static auto h = std::hash<PyObject*>{};
			return h(py::handle(scriptClass).ptr());
		}
	};
}

namespace Cardia
{

	class ScriptEngine
	{
	public:
		ScriptEngine();
		~ScriptEngine();
		ScriptClass GetClassFromPyFile(const std::string& name, const std::string& content);
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
