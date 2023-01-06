#pragma once

#include <Cardia.hpp>
#include <entt/entt.hpp>

#include "IPanel.hpp"


namespace Cardia :: Panel
{
	class SceneHierarchyPanel : public IPanel
	{
	public:
		SceneHierarchyPanel() = default;
                ~SceneHierarchyPanel() override = default;
		void OnImGuiRender() override;
		void SetSelectedEntity(Entity entity);
		Entity GetSelectedEntity() { return m_SelectedEntity; }

	private:
		void drawHierarchy();

	private:
		Entity m_SelectedEntity;
	};
}
