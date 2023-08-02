#pragma once

#include <Cardia.hpp>
#include <entt/entt.hpp>

#include "IPanel.hpp"


namespace Cardia :: Panel
{
	class SceneHierarchyPanel : public IPanel
	{
	public:
		explicit SceneHierarchyPanel(PanelManager* manager) : IPanel(manager, m_LastWindowId++) {}
		~SceneHierarchyPanel() override = default;
		void OnImGuiRender(CardiaTor* appContext) override;
		void SetSelectedEntity(Entity entity);
		void OnSceneLoad(Scene* scene) override;

	private:
		void DrawEntityNode(Entity entity, const Component::Relationship& relationship, CardiaTor* appCtx);
		void SetSelectedEntityFromItself(Entity entity, CardiaTor* appCtx);

	private:
		Entity m_SelectedEntity;

		static int m_LastWindowId;
	};
}
