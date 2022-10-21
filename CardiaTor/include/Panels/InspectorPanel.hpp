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
                void OnImGuiRender() override;
		void OnSceneLoad(Cardia::Scene *scene) override;


        private:
                template<typename T>
                void drawInspectorComponent(const char* name, Entity entity, std::function<void(T&)> func);
		Scene* m_CurrentScene;
        };
}
