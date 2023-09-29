#pragma once

#include "Cardia/Renderer/MeshRenderer.hpp"

namespace Cardia::Component
{
	struct ModelRenderer
	{
		ModelRenderer() = default;

		std::shared_ptr<MeshRenderer> Renderer = nullptr;

		inline void Reset() {
			Renderer = nullptr;
		}

		Json::Value Serialize() const;
		static std::optional<ModelRenderer> Deserialize(const Json::Value& root);

	private:
	};
}
