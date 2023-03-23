#pragma once
#include "IPanel.hpp"
#include "Cardia/Core/Time.hpp"


namespace Cardia::Panel
{
	class DebugPanel : public IPanel
	{
	public:
		explicit DebugPanel(PanelManager* manager) : IPanel(manager, m_LastWindowId++) {}
		~DebugPanel() override = default;
		void OnImGuiRender(CardiaTor* appContext) override;
	private:
		static int m_LastWindowId;
	};
}
