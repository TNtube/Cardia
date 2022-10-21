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
		void OnImGuiRender() override;
		void OnSceneLoad(Cardia::Scene *scene) override;

	private:
		void drawHierarchy();

	private:
		Scene* m_Scene;
	};
}
