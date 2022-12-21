#pragma once
#include <functional>

#include "IPanel.hpp"
#include "Cardia/ECS/Entity.hpp"
#include "Cardia/Scripting/ScriptInstance.hpp"
#include "Cardia/Scripting/ScriptClass.hpp"


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
                void DrawInspectorComponent(const char* name, Entity entity, std::function<void(T&)> func);
		bool DrawField(ScriptInstance* behaviorInstance, std::pair<std::string, ScriptField>& fieldData);
		Scene* m_CurrentScene {nullptr};
        };
}
