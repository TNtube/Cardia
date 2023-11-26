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

		constexpr static auto properties = std::make_tuple(
			// TODO: review this
		);
	};
}
