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
		void pushLayer(Layer* layer);
		void pushOverlay(Layer* overlay);
		void popLayer(const Layer* layer);
		void popOverlay(const Layer* overlay);

		auto begin() { return m_Layers.begin(); }
		auto end() { return m_Layers.end(); }
		auto rbegin() { return m_Layers.rbegin(); }
		auto rend() { return m_Layers.rend(); }

	private:
		std::vector<Layer*> m_Layers;
		std::vector<Layer*>::iterator m_LayerInsert;
	};
}
