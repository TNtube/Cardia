#include "cdpch.hpp"
#include "Cardia/ECS/Component/Light.hpp"
#include "Cardia/Math/Vector4.hpp"

namespace Cardia::Component
{

	Json::Value Light::Serialize() const
	{
		Json::Value root;

		auto& light = root["Light"];
		light["LightType"] = LightType;
		light["Color"] = Color.Serialize();
		light["Range"] = Range;
		light["Angle"] = Angle;
		light["Smoothness"] = Smoothness;

		return root;
	}

	std::optional<Light> Light::Deserialize(const Json::Value& root)
	{
		if (!root.isMember("Light"))
			return std::nullopt;

		const auto& lightNode = root["Light"];
		if (!lightNode.isMember("LightType") ||
			!lightNode.isMember("Color") ||
			!lightNode.isMember("Range") ||
			!lightNode.isMember("Angle") ||
			!lightNode.isMember("Smoothness"))
				return std::nullopt;

		Light temp;

		if (const auto color = Vector4f::Deserialize(lightNode["Color"]))
			temp.Color = color.value();
		else
			return std::nullopt;

		temp.LightType = lightNode["LightType"].asInt();
		temp.Range = lightNode["Range"].asFloat();
		temp.Angle = lightNode["Angle"].asFloat();
		temp.Smoothness = lightNode["Smoothness"].asFloat();

		return temp;
	}
}