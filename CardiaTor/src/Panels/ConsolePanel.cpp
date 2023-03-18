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


		ImGui::End();
	}
}
