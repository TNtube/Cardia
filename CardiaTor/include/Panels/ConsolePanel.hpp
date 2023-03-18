#pragma once
#include "IPanel.hpp"

namespace Cardia::Panel
{
	class ConsolePanel : public IPanel
	{
	public:
		explicit ConsolePanel(PanelManager* manager) : IPanel(manager, m_LastWindowId++){}
		~ConsolePanel() override = default;
		void OnImGuiRender(CardiaTor* appContext) override;
	private:

		static int m_LastWindowId;
	};
}