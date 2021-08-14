#pragma once

#include "Layer.hpp"
#include "Utopia/Core.hpp"

namespace Utopia
{
	class UTOPIA_API LayerStack
	{
	public:
		LayerStack();
		~LayerStack();
		void pushLayer(std::shared_ptr<Layer>& layer);
		void pushOverlay(std::shared_ptr<Layer>& overlay);
		void popLayer(const std::shared_ptr<Layer>& layer);
		void popOverlay(const std::shared_ptr<Layer>& overlay);

		auto begin() { return m_Layers.begin(); }
		auto end() { return m_Layers.end(); }
		auto rbegin() { return m_Layers.rbegin(); }
		auto rend() { return m_Layers.rend(); }

	private:
		std::vector<std::shared_ptr<Layer>> m_Layers;
		std::vector<std::shared_ptr<Layer>>::iterator m_LayerInsert;
	};
}
