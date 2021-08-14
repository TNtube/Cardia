#include "utpch.hpp"

#include "LayerStack.hpp"

namespace Utopia
{
	LayerStack::LayerStack()
	{
		m_LayerInsert = m_Layers.begin();
	}

	LayerStack::~LayerStack()
	{
	}


	void LayerStack::pushLayer(std::shared_ptr<Layer>& layer)
	{
		m_Layers.emplace(m_LayerInsert, layer);
		++m_LayerInsert;
	}

	void LayerStack::popLayer(const std::shared_ptr<Layer>& layer)
	{
		const auto it = std::find(m_Layers.begin(), m_Layers.end(), layer);
		if(it != m_Layers.end())
		{
			m_Layers.erase(it);
			--m_LayerInsert;
		}
	}

	void LayerStack::pushOverlay(std::shared_ptr<Layer>& overlay)
	{
		m_Layers.emplace_back(overlay);
	}

	void LayerStack::popOverlay(const std::shared_ptr<Layer>& overlay)
	{
		const auto it = std::find(m_Layers.begin(), m_Layers.end(), overlay);
		if (it != m_Layers.end())
		{
			m_Layers.erase(it);
			--m_LayerInsert;
		}

	}



}
