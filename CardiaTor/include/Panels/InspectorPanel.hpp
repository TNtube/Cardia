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
	        explicit InspectorPanel(PanelManager* manager) : IPanel(manager), m_WindowId(m_LastWindowId++) {}
                ~InspectorPanel() override = default;
                void OnImGuiRender(CardiaTor* appContext) override;
		void SetSelectedEntity(Entity entity);
		void OnSceneLoad(Scene* scene) override;


        private:
                template<typename T>
                void DrawInspectorComponent(const char* name, std::function<void(T&)> func);
	        bool DrawField(ScriptInstance* behaviorInstance, ScriptFieldType type, const char* fieldName, py::object& field);
	        Entity m_SelectedEntity;

		bool m_IsOpen = true;

		int m_WindowId;

		static int m_LastWindowId;
        };
}
