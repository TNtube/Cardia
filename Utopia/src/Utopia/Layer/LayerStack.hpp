#pragma once

#include "Layer.hpp"
#include "Utopia/Core.hpp"

namespace Utopia
{
	class LayerStack
	{
	public:
		LayerStack() = default;
		~LayerStack() = default;
		void pushLayer(Layer* layer);
		void pushOverlay(Layer* overlay);
		void popLayer(Layer* layer);
		void popOverlay(Layer* overlay);

		auto begin() { return m_Layers.begin(); }
		auto end() { return m_Layers.end(); }
		auto rbegin() { return m_Layers.rbegin(); }
		auto rend() { return m_Layers.rend(); }

	private:
		std::vector<Layer*> m_Layers;
		uint32_t m_LayerInsertIndex = 0;
	};
}
