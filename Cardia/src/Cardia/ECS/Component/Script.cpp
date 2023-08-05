#include "cdpch.hpp"

#include "Cardia/ECS/Component/Script.hpp"
#include "Cardia/Core/Log.hpp"
#include "Cardia/Scripting/ScriptEngine.hpp"


namespace Cardia::Component
{
	Json::Value Script::Serialize() const
	{
		Json::Value root;
		auto& script = root["Script"];
		
		script["Path"] = GetPath();

		for (auto& item : Class.Attributes()) {
			script["Attributes"].append(item.Serialize());
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
		auto& attrs = temp.Class.Attributes();
		for (const auto& subNode: attrsNode) {
			auto field = ScriptField::Deserialize(subNode);
			if (!field) continue;
			auto attrPair = std::ranges::find_if(attrs, [&](const ScriptField& attr) {
				return attr == field.value();
			});
			if (attrPair != attrs.end()) {
				*attrPair = field.value();
			}
		}

		return temp;
	}

	void Script::ReloadFile()
	{
		if (m_Path.empty()) return;
		Log::Info("Loading path {0}", m_Path);
		const std::ifstream t(m_Path);
		std::stringstream buffer;
		buffer << t.rdbuf();
		auto filePath = std::filesystem::path(m_Path);
		Class = ScriptEngine::Instance().GetClassFromPyFile(filePath);
		Class.RegisterAttributes();
	}
}
