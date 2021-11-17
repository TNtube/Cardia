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

	private:
		template<typename T>
		void drawInspectorComponent(const char* name, std::function<void(T&)> func);
		void drawHierarchy();
		void drawComponents();
		void drawPopupAddComponent();
		void resetEntityClicked() { m_EntityClicked = {}; }

	private:
		Scene* m_Scene;
		Entity m_EntityClicked;
	};
}