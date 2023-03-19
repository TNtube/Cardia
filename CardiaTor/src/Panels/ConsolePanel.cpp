#include <imgui.h>
#include "Panels/ConsolePanel.hpp"
#include "Panels/PanelManager.hpp"


namespace Cardia::Panel
{

	int ConsolePanel::m_LastWindowId = 0;
	void ConsolePanel::OnImGuiRender(CardiaTor *appContext)
	{
		char buff[64];
		sprintf(buff, "Console##%i", m_WindowId);

		ImGui::SetNextWindowSize(ImVec2(500, 300), ImGuiCond_FirstUseEver);
		if (!ImGui::Begin(buff, &m_IsOpen)) {
			ImGui::End();
			return;
		}

		if (ImGui::Button("Clear")) Clear();
		ImGui::SameLine();
		bool copy = ImGui::Button("Copy");
		ImGui::SameLine();
		m_Filter.Draw("Filter", -100.0f);
		ImGui::Separator();
		ImGui::BeginChild("scrolling");
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0,1));
		if (copy) ImGui::LogToClipboard();

		for (const auto& line : m_Lines) {
			if (!m_Filter.IsActive() || m_Filter.PassFilter(line.line.c_str())) {
				switch (line.level)
				{
					case spdlog::level::info:
						ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0,255,0,255));
						break;
					case spdlog::level::warn:
						ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255,165,0,255));
						break;
					case spdlog::level::err:
						ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255,0,0,255));
						break;
					default:
						ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0,0,0,255));
						break;
				}
				ImGui::Text("%s", line.line.c_str());
				ImGui::PopStyleColor();
			}
		}

		if (m_ScrollToBottom)
			ImGui::SetScrollHereY(1.0f);
		m_ScrollToBottom = false;
		ImGui::PopStyleVar();
		ImGui::EndChild();


		ImGui::End();
	}

	void ConsolePanel::AddLog(const spdlog::level::level_enum& level, const std::string& fmt)
	{
		m_Lines.push_back({fmt, level});
		m_ScrollToBottom = true;
	}
}
