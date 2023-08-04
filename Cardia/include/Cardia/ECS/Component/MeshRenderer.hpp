#pragma once

#include "Cardia/Renderer/MeshRenderer.hpp"

namespace Cardia::Component
{
	struct MeshRendererC
	{
		MeshRendererC() : Renderer(std::make_shared<MeshRenderer>()) {}

		std::shared_ptr<MeshRenderer> Renderer = nullptr;

		inline void Reset() {
			Renderer = nullptr;
		}

		Json::Value Serialize() const;
		static std::optional<MeshRendererC> Deserialize(const Json::Value& root);
	};
}
