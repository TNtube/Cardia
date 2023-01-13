#pragma once
#include "IPanel.hpp"
#include "Cardia/Core/Time.hpp"


namespace Cardia::Panel
{
        class DebugPanel : public IPanel
        {
        public:
		explicit DebugPanel(PanelManager* manager) : IPanel(manager), m_WindowId(m_LastWindowId++) {}
                ~DebugPanel() override = default;
                void OnImGuiRender(CardiaTor* appContext) override;
        private:
		bool m_IsOpen = true;

	        int m_WindowId;

	        static int m_LastWindowId;
        };        
}
