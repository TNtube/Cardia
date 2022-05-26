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
		void onImGuiRender(DeltaTime deltaTime) override;
		Entity& getClickedEntity() { return m_EntityClicked; }
		void setCurrentScene(Scene* scene) { m_Scene = scene; }

	private:
		void drawHierarchy();
		void resetEntityClicked() { m_EntityClicked = {}; }

	private:
		Scene* m_Scene;
		Entity m_EntityClicked;
	};
}
