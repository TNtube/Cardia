#pragma once
#include <imgui.h>
#include <Cardia.hpp>

namespace Cardia :: Panel
{
	class SceneHierarchy
	{
	public:
		explicit SceneHierarchy(Scene* scene);
		void onImGuiRender(DeltaTime deltaTime);
		void drawHierarchy();
		void drawComponents();

	private:
		Scene* m_Scene;
		std::unique_ptr<Entity> m_EntityClicked;
	};
}