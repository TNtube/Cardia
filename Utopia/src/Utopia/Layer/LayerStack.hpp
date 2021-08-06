#pragma once

#include "Layer.hpp"
#include "Utopia/Core.hpp"

namespace Utopia
{
	using LayerPtr = std::shared_ptr<Layer>;
	class UTOPIA_API LayerStack
	{
	public:
		LayerStack();
		~LayerStack();
		void pushLayer(LayerPtr layer);
		void pushOverlay(LayerPtr overlay);
		void popLayer(LayerPtr layer);
		void popOverlay(LayerPtr overlay);


		auto begin() { return m_Layers.begin(); }
		auto end() { return m_Layers.end(); }
		auto rbegin() { return m_Layers.rbegin(); }
		auto rend() { return m_Layers.rend(); }

	private:
		std::vector<LayerPtr> m_Layers;
		std::vector<LayerPtr>::iterator m_LayerInsert;
	};
}
