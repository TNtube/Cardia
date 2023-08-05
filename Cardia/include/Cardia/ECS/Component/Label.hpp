#pragma once

#include "Cardia/Math/Vector4.hpp"

namespace Cardia::Component
{
	struct Label
	{
		Label() = default;
		explicit Label(std::string Label)
			: Name(std::move(Label)) {}

		std::string Name;
		Vector4f Color { 1.0f };

		inline void Reset() {
			Name = "Default";
			Color = Vector4f(1.0f);
		}

		Json::Value Serialize() const;
		static std::optional<Label> Deserialize(const Json::Value& root);
	};
}