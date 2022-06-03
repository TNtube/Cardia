#pragma once
#include <functional>

#include "IPanel.hpp"
#include "Cardia/ECS/Entity.hpp"


namespace Cardia::Panel
{
        class InspectorPanel : public IPanel
        {
        public:
                InspectorPanel() = default;
                ~InspectorPanel() override = default;
                void onImGuiRender(DeltaTime deltaTime) override;

        private:
                template<typename T>
                void drawInspectorComponent(const char* name, std::function<void(T&)> func);
        };
}
