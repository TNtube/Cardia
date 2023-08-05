#include "cdpch.hpp"
#include "Cardia/ECS/Component/Label.hpp"

namespace Cardia::Component
{
	Json::Value Label::Serialize() const
	{
		Json::Value root;

		auto& label = root["Label"];

		label["Name"] = Name;
		label["Color"] = Color.Serialize();

		return root;
	}

	std::optional<Label> Label::Deserialize(const Json::Value& root)
	{
		if (!root.isMember("Label"))
			return std::nullopt;

		const auto& label = root["Label"];

		if (!label.isMember("Name") ||
			!label.isMember("Color"))
				return std::nullopt;

		Label temp;

		temp.Name = label["Name"].asString();

		if (const auto& color = Vector4f::Deserialize(label["Color"]))
			temp.Color = color.value();
		else
			return std::nullopt;

		return temp;
	}
}