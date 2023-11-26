#pragma once

#include "Cardia/Math/Vector3.hpp"
#include "Cardia/Serialization/Serializable.hpp"

namespace Cardia::Component
{
	namespace LightType {
		enum LightType : int32_t {
			DirectionalLight = 0,
			PointLight,
			SpotLight
		};
	}

	struct Light
	{
		Light() = default;
		int32_t LightType {};
		Vector3f Color {};
		float Range = 2;
		float Angle = 35;
		float Smoothness = 1;

		inline void Reset() {
			Color = Vector3f(0);
			Range = 2;
			Angle = 35;
			Smoothness = 1;
		}

		constexpr static auto properties = std::make_tuple(
			property(&Light::LightType, "LightType"),
			property(&Light::Color, "Color"),
			property(&Light::Range, "Range"),
			property(&Light::Angle, "Angle"),
			property(&Light::Smoothness, "Smoothness")
		);
	};
}
