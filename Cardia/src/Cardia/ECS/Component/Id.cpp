#include "cdpch.hpp"
#include "Cardia/ECS/Component/Id.hpp"

namespace Cardia::Component
{
	
	Json::Value ID::Serialize() const
	{
		Json::Value root;

		root["ID"] = Uuid.ToString();

		return root;
	}

	std::optional<ID> ID::Deserialize(const Json::Value& root)
	{
		if (!root.isMember("ID"))
			return std::nullopt;

		ID temp;
		temp.Uuid = UUID::FromString(root["ID"].asString());

		return temp;
	}
}