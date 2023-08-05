#pragma once
#include <typeindex>
#include <algorithm>
#include <memory>
#include <vector>
#include <unordered_map>
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
			auto& panelsT = m_Panels[typeid(T)];
			auto it = std::find_if(panelsT.begin(), panelsT.end(), [&](std::unique_ptr<Panel::IPanel>& panel) {
				return !panel->IsOpen();
			});
			if (it != panelsT.end())
			{
				T* panelPtr = reinterpret_cast<T*>(it->get());
				panelPtr->Open();
				return panelPtr;
			}


			auto& panel = panelsT.emplace_back(std::make_unique<T>(this));
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

		void CloseAllPanels()
		{
			m_Panels.clear();
			m_LastFocusedPanels.clear();
		}

		inline std::vector<Panel::IPanel*> Panels() {
			std::vector<Panel::IPanel*> panels;
			for (auto& pair: m_Panels)
			{
				panels.reserve(panels.size() + pair.second.size());
				for (auto& ptr: pair.second)
				{
					panels.push_back(ptr.get());
				}
			}
			return panels;
		}

	private:
		std::unordered_map<std::type_index, std::vector<std::unique_ptr<Panel::IPanel>>> m_Panels;
		std::unordered_map<std::type_index, Panel::IPanel*> m_LastFocusedPanels;
	};
}