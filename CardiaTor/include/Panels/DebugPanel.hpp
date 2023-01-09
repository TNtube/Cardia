#pragma once
#include "IPanel.hpp"
#include "Cardia/Core/Time.hpp"


namespace Cardia::Panel
{
        class DebugPanel : public IPanel
        {
        public:
		explicit DebugPanel(PanelManager* manager) : IPanel(manager) {}
                ~DebugPanel() override = default;
                void OnImGuiRender() override;
        };        
}
