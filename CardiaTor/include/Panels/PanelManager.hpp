#pragma once
#include <typeindex>
#include <type_traits>
#include "IPanel.hpp"

namespace Cardia
{
	class PanelManager
	{
	public:
		PanelManager();


		template<typename T, std::enable_if_t<std::is_base_of_v<Panel::IPanel, T>, bool> = true>
		T* CreatePanel()
		{
			auto& panel = m_Panels.emplace_back(std::make_unique<T>(this));
			T* panelPtr = reinterpret_cast<T*>(panel.get());
			m_LastFocusedPanels.insert_or_assign(typeid(T), panelPtr);
			return panelPtr;
		}

		template<typename T, std::enable_if_t<std::is_base_of_v<Panel::IPanel, T>, bool> = true>
		T* GetLastFocused()
		{
			auto lastFocused = m_LastFocusedPanels.find(typeid(T));
			if (lastFocused == m_LastFocusedPanels.end())
				return nullptr;

			return reinterpret_cast<T*>(lastFocused->second);
		}

		template<typename T, std::enable_if_t<std::is_base_of_v<Panel::IPanel, T>, bool> = true>
		void SetFocused(T *panel)
		{
			m_LastFocusedPanels.insert_or_assign(typeid(T), panel);
		}

		template<typename T, std::enable_if_t<std::is_base_of_v<Panel::IPanel, T>, bool> = true>
		void DeletePanel(T* panel)
		{
			auto panelToDelete = std::find_if(m_Panels.begin(), m_Panels.end(), [panel](auto& currPanel) {
				return currPanel.get() == panel;
			});

			if (panelToDelete == m_Panels.end())
				return;

			m_LastFocusedPanels.erase(typeid(T));
			m_Panels.erase(panelToDelete);
		}

		inline std::vector<std::unique_ptr<Panel::IPanel>>& Panels() { return m_Panels; }

	private:
		std::vector<std::unique_ptr<Panel::IPanel>> m_Panels;
		std::unordered_map<std::type_index, Panel::IPanel*> m_LastFocusedPanels;
	};
}