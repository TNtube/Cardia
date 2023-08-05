#pragma once

#include "Cardia/Math/Vector4.hpp"
#include "Cardia/Renderer/Texture.hpp"

namespace Cardia::Component
{
	struct SpriteRenderer
	{
		SpriteRenderer() = default;
		explicit SpriteRenderer(const Vector4f& color)
			: Color(color) {}

		Vector4f Color { 1.0f };
		std::shared_ptr<Texture2D> Texture = nullptr;
		float TillingFactor = 1.0f;
		int32_t ZIndex = 0;

		inline void Reset() {
			Texture = nullptr;
			TillingFactor = 1.0f;
			Color = Vector4f(1.0f);
		}

		Json::Value Serialize() const;
		static std::optional<SpriteRenderer> Deserialize(const Json::Value& root);
	};
}