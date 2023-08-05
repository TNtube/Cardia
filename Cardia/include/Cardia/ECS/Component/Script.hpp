#pragma once

#include "Cardia/Scripting/ScriptClass.hpp"
#include <json/json.h>

namespace Cardia::Component
{
	struct Script
	{
		Script() = default;
		explicit Script(std::string scriptPath) : m_Path(std::move(scriptPath)) {
			ReloadFile();
		}
		inline std::string GetPath() const {return m_Path; }
		inline void SetPath(const std::string& newPath) { m_Path = newPath; ReloadFile(); }
		ScriptClass Class;

		inline void Reset()
		{
			m_Path.clear();
			Class = ScriptClass();
		}

		Json::Value Serialize() const;
		static std::optional<Script> Deserialize(const Json::Value& root);

	private:
		void ReloadFile();
		std::string m_Path;
	};
}
