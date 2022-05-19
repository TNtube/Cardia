#pragma once
#include <functional>

#include "IPanel.hpp"
#include "Cardia/ECS/Entity.hpp"


namespace Cardia::Panel
{
        class InspectorPanel : public IPanel
        {
        public:
                InspectorPanel(Entity& entity) : m_SelectedEntity(entity) {}
                ~InspectorPanel() = default;
                void onImGuiRender(DeltaTime deltaTime) override;
                void updateSelectedEntity(const Entity& entity) const;

        private:
                template<typename T>
                void drawInspectorComponent(const char* name, std::function<void(T&)> func);

        private:
                Entity& m_SelectedEntity;
        };
}
