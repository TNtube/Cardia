#include <Cardia/Application.hpp>
#include "cdpch.hpp"

#include "Cardia/ECS/Component/Script.hpp"

#include "Cardia/Assets/AssetsManager.hpp"
#include "Cardia/Core/Log.hpp"
#include "Cardia/Scripting/ScriptEngine.hpp"


namespace Cardia::Component
{
	void Script::ReloadFile()
	{
		if (m_Path.empty()) return;
		auto& am = Application::Get().GetAssetsManager();
		m_File = ScriptFile::FromPath(am.AbsolutePathFromUUID(am.GetUUIDFromAsset(m_Path)));
	}
}
