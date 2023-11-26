#pragma once

#include "Cardia/Scripting/ScriptFile.hpp"
#include <json/json.h>

namespace Cardia::Component
{
	struct Script
	{
		Script() = default;
		explicit Script(std::string scriptPath) : m_Path(std::move(scriptPath)) {
			ReloadFile();
		}

		bool IsLoaded() const { return m_File != nullptr; }

		std::string GetPath() const { return m_Path; }
		void SetPath(const std::string& newPath) { m_Path = newPath; ReloadFile(); }

		ScriptFile& GetFile() const { return *m_File; }

		void Reset()
		{
			m_Path.clear();
			m_File.reset();
		}

	private:
		void ReloadFile();
		std::string m_Path;
		std::shared_ptr<ScriptFile> m_File;

	public:
		constexpr static auto properties = std::make_tuple(
			// TODO: review this
		);
	};
}
