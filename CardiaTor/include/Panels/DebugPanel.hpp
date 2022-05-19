#pragma once
#include "IPanel.hpp"
#include "Cardia/Core/Time.hpp"


namespace Cardia::Panel
{
        class DebugPanel : public IPanel
        {
        public:
                DebugPanel() = default;
                ~DebugPanel() override = default;
                void onImGuiRender(DeltaTime deltaTime) override;
        };        
}
