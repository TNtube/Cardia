#include "cdpch.hpp"

#include "Cardia/ECS/Component/Script.hpp"

#include "Cardia/Asset/AssetsManager.hpp"
#include "Cardia/Core/Log.hpp"
#include "Cardia/Scripting/ScriptEngine.hpp"


namespace Cardia::Component
{
	Json::Value Script::Serialize() const
	{
		if (!m_File) return {};

		Json::Value root;
		auto& script = root["Script"];
		
		script["Path"] = GetPath();

		for (auto& [name, field] : m_File->Attributes()) {
			script["Attributes"].append(field.Serialize());
		}

		return root;
	}

	std::optional<Script> Script::Deserialize(const Json::Value& root)
	{
		if (!root.isMember("Script"))
			return std::nullopt;

		auto& script = root["Script"];

		if (!script.isMember("Path"))
			return std::nullopt;

		Script temp;

		temp.SetPath(script["Path"].asString());

		if (!script.isMember("Attributes"))
			return temp;

		auto& attrsNode = script["Attributes"];
		auto& attrs = temp.m_File->Attributes();
		for (const auto& subNode: attrsNode) {
			auto field = ScriptField::Deserialize(subNode);
			if (!field) continue;

			if (attrs.contains(field->GetName())) {
				attrs.insert_or_assign(field->GetName(), *field);
			} else {
				Log::CoreWarn("Deserialization: ScriptField '{0}' not found in script '{1}'", field->GetName(), temp.GetPath());
			}
		}

		return temp;
	}

	void Script::ReloadFile()
	{
		if (m_Path.empty()) return;
		m_File = ScriptFile::FromPath(AssetsManager::GetAssetAbsolutePath(m_Path));
	}
}
