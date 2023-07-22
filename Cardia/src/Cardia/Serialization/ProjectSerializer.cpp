#include "cdpch.hpp"

#include "Cardia/Serialization/ProjectSerializer.hpp"
#include <json/json.h>

namespace Cardia::Serialization
{

	bool ProjectSerializer::Serialize(const std::filesystem::path &path)
	{
		std::ofstream file(path);
		if (!file.is_open())
			return false;

		Json::Value root;

		auto& proj = root["Project"];
		auto& config = m_Project.GetConfig();

		proj["Name"] = config.Name;
		proj["AssetsDirectory"] = config.AssetDirectory.string();
		proj["StartScene"] = config.StartScene.string();

		file << root;

		return true;
	}

	bool ProjectSerializer::Deserialize(const std::filesystem::path &path)
	{

		Json::Value root;
		std::ifstream file(path);

		std::string err;
		const Json::CharReaderBuilder builder;
		if (!file.is_open() || !Json::parseFromStream(builder, file, &root, &err))
		{
			Log::CoreError("Could not load m_Project {0} : {1}", path.string(), err);
			return false;
		}

		auto& proj = root["Project"];
		auto& config = m_Project.GetConfig();

		config.Name = proj["Name"].asString();
		config.AssetDirectory = proj["AssetsDirectory"].asString();
		config.StartScene = proj["StartScene"].asString();

		return true;
	}
}
