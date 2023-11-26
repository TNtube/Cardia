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
		std::shared_ptr<Texture> SpriteTexture = nullptr;
		float TillingFactor = 1.0f;
		int32_t ZIndex = 0;

		inline void Reset() {
			SpriteTexture = nullptr;
			TillingFactor = 1.0f;
			Color = Vector4f(1.0f);
			ZIndex = 0;
		}


		constexpr static auto properties = std::make_tuple(
			property(&SpriteRenderer::Color, "Color"),
			// property(&SpriteRenderer::SpriteTexture, "SpriteTexture"), TODO: review this
			property(&SpriteRenderer::TillingFactor, "TillingFactor"),
			property(&SpriteRenderer::ZIndex, "ZIndex")
		);
	};
}