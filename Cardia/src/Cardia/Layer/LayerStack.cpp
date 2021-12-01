#include "cdpch.hpp"
#include "Cardia/Layer/LayerStack.hpp"


namespace Cardia
{
	void LayerStack::pushLayer(Layer* layer)
	{
		layer->onPush();
		m_Layers.emplace(m_Layers.begin() + m_LayerInsertIndex, layer);
		++m_LayerInsertIndex;
	}

	void LayerStack::popLayer(Layer* layer)
	{
		auto it = std::find(m_Layers.begin(), m_Layers.end(), layer);
		if(it != m_Layers.begin() + m_LayerInsertIndex)
		{
			layer->onPop();
			m_Layers.erase(it);
			--m_LayerInsertIndex;
		}
	}

	void LayerStack::pushOverlay(Layer* overlay)
	{
		overlay->onPush();
		m_Layers.emplace_back(overlay);
	}

	void LayerStack::popOverlay(Layer* overlay)
	{
		const auto it = std::find(m_Layers.begin(), m_Layers.end(), overlay);
		if (it != m_Layers.end())
		{
			overlay->onPop();
			m_Layers.erase(it);
		}
	}
}
