#pragma once
#include <imgui.h>
#include <Cardia.hpp>
#include <entt/entt.hpp>

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
		entt::entity m_EntityClicked { entt::null };
	};
}