#pragma once

#include <Cardia.hpp>
#include <entt/entt.hpp>

#include "IPanel.hpp"


namespace Cardia :: Panel
{
	class SceneHierarchyPanel : public IPanel
	{
	public:
		explicit SceneHierarchyPanel(PanelManager* manager) : IPanel(manager) {}
                ~SceneHierarchyPanel() override = default;
		void OnImGuiRender() override;
		void SetSelectedEntity(Entity entity);
		void OnSceneLoad(Scene* scene) override;
		Entity GetSelectedEntity() { return m_SelectedEntity; }

	private:
		void drawHierarchy();

	private:
		Entity m_SelectedEntity;
	};
}
