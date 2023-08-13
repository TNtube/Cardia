﻿#pragma once
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
		explicit InspectorPanel(PanelManager* manager) : IPanel(manager, m_LastWindowId++){}
		~InspectorPanel() override = default;
		void OnImGuiRender(CardiaTor* appContext) override;
		void SetSelectedEntity(Entity entity);
		void OnSceneLoad(Scene* scene) override;


	private:
		template<typename T>
		void DrawInspectorComponent(const char* name, std::function<void(T&)> func);
		static bool DrawField(ScriptField& scriptField);
		Entity m_SelectedEntity;

		static int m_LastWindowId;
	};
}
