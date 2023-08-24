#pragma once

#include <pybind11/pybind11.h>

#include "ScriptClass.hpp"
#include "EntityBehavior.hpp"
#include "ScriptUtils.hpp"
#include "Cardia/ECS/Component/Id.hpp"

namespace py = pybind11;

namespace Cardia
{
	class Scene;

	class ScriptFile
	{
	public:
		ScriptFile(std::string path, py::object ast, py::dict locals);
		static std::shared_ptr<ScriptFile> FromSource(const std::string& source, const std::string& filename = "default.py");
		static std::shared_ptr<ScriptFile> FromPath(const std::filesystem::path& absolutePath);

		ScriptField* GetScriptField(const std::string& name);
		void SetScriptField(const std::string& name, const ScriptField& field);
		bool HasScriptField(const std::string& name);

		template<typename T>
		T GetAttribute(const std::string& name)
		{
			if (m_BehaviorPtr)
				return m_BehaviorInstance.attr(name.c_str()).cast<T>();
			return GetScriptField(name)->GetValue<T>();
		}

		template<typename T>
		void SetAttribute(const std::string& name, T value)
		{
			if (m_BehaviorPtr)
				m_BehaviorInstance.attr(name.c_str()) = value;
			else
				GetScriptField(name)->SetValue(py::cast(value));
		}

		std::optional<Component::ID> GetBehaviorAttribute(const std::string& name);
		void SetBehaviorAttribute(const std::string& name, Entity entity);

		bool HasBehavior() const { return IsSubclass<Behavior>(m_BehaviorClassDef); }
		Behavior* GetBehavior() const { return m_BehaviorPtr; }

		Behavior* InstantiateBehavior(Entity entity);
		void ResolveBehaviorReferences(Scene& scene);
		void DestroyBehavior();

		std::vector<ScriptField>& Attributes() { return m_Attributes; }
	private:
		void RetrieveScriptInfos();

		std::string m_Filename;
		py::object m_Ast;
		py::dict m_Locals;

		py::type m_BehaviorClassDef;
		py::object m_BehaviorInstance;
		Behavior* m_BehaviorPtr = nullptr;

		std::vector<ScriptField> m_Attributes;
	};
}
