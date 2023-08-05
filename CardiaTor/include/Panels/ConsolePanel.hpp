#pragma once
#include "IPanel.hpp"
#include <imgui.h>
#include <Cardia/Core/Log.hpp>

namespace Cardia::Panel
{
	class ConsolePanel : public IPanel
	{
	public:
		explicit ConsolePanel(PanelManager* manager) : IPanel(manager, m_LastWindowId++){}
		~ConsolePanel() override = default;
		void OnImGuiRender(CardiaTor* appContext) override;
		void AddLog(const spdlog::level::level_enum& level, const std::string& fmt);
	private:
		void Clear()     { m_Lines.clear(); }

		struct LineLevel
		{
			std::string line;
			spdlog::level::level_enum level;
		};
		std::vector<LineLevel> m_Lines;
		ImGuiTextFilter m_Filter;
		bool m_ScrollToBottom {};
		static int m_LastWindowId;
	};
}