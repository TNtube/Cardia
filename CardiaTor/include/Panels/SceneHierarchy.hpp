#pragma once

#include <Cardia.hpp>
#include <entt/entt.hpp>

#include "IPanel.hpp"


namespace Cardia :: Panel
{
	class SceneHierarchy : public IPanel
	{
	public:
		explicit SceneHierarchy(Scene* scene);
                ~SceneHierarchy() override = default;
		void onImGuiRender() override;
		void setCurrentScene(Scene* scene) { m_Scene = scene; resetEntityClicked(); }

	private:
		void drawHierarchy();
		void resetEntityClicked() { m_SelectedEntity = {}; }

	private:
		Scene* m_Scene;
	};
}
