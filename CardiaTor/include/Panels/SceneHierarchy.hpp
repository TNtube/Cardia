#pragma once

#include <Cardia.hpp>
#include <entt/entt.hpp>


namespace Cardia :: Panel
{
	class SceneHierarchy
	{
	public:
		explicit SceneHierarchy(Scene* scene);
		void onImGuiRender(DeltaTime deltaTime);
		Entity& getClickedEntity() { return m_EntityClicked; }

	private:
		void drawHierarchy();
		void resetEntityClicked() { m_EntityClicked = {}; }

	private:
		Scene* m_Scene;
		Entity m_EntityClicked;
	};
}