#include "cdpch.hpp"
#include "Cardia/Asset/AssetsManager.hpp"
#include "Cardia/ECS/Component/SpriteRenderer.hpp"

namespace Cardia::Component
{
	Json::Value SpriteRenderer::Serialize() const
	{
		Json::Value root;

		auto& sprite = root["SpriteRenderer"];

		sprite["Color"] = Color.Serialize();
		sprite["Texture"] = AssetsManager::GetPathFromAsset(Texture).string();
		sprite["TilingFactor"] = TillingFactor;
		sprite["ZIndex"] = ZIndex;

		return root;
	}

	std::optional<SpriteRenderer> SpriteRenderer::Deserialize(const Json::Value& root)
	{
		if (!root.isMember("SpriteRenderer"))
			return std::nullopt;

		const auto& sprite = root["SpriteRenderer"];

		if (!sprite.isMember("Color") ||
			!sprite.isMember("Texture") ||
			!sprite.isMember("TilingFactor") ||
			!sprite.isMember("ZIndex"))
				return std::nullopt;

		SpriteRenderer temp;

		if (const auto color = Vector4f::Deserialize(sprite["Color"]))
			temp.Color = color.value();
		else
			return std::nullopt;

		temp.Texture = AssetsManager::Load<Texture2D>(sprite["Texture"].asString());
		temp.TillingFactor = sprite["TilingFactor"].asFloat();
		temp.ZIndex = sprite["ZIndex"].asInt();

		return temp;
	}
}