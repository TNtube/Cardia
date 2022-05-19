#pragma once


#include <Cardia.hpp>
#include <random>

#include "EditorCamera.hpp"
#include "Panels/SceneHierarchy.hpp"
#include "Panels/DebugPanel.hpp"
#include "Panels/FileHierarchy.hpp"
#include "Panels/InspectorPanel.hpp"


namespace Cardia
{
	class CardiaTor : public Application
	{
	public:
		CardiaTor();

		~CardiaTor() override = default;
		void onUpdate(DeltaTime deltaTime) override;
		void onEvent(Event& event) override;
		void onImGuiDraw(DeltaTime deltaTime) override;

	private:
		void enableDocking();
		std::unique_ptr<Texture2D> m_TextureSquare;
		std::unique_ptr<Texture2D> m_TextureBox;
		std::unique_ptr<Framebuffer> m_Framebuffer;

		std::unique_ptr<Scene> m_CurrentScene;
		std::unique_ptr<Panel::SceneHierarchy> m_SceneHierarchyPanel;
		std::unique_ptr<Panel::InspectorPanel> m_InspectorPanel;
		std::unique_ptr<Panel::DebugPanel> m_DebugPanel;
		std::unique_ptr<Panel::FileHierarchy> m_FileHierarchyPanel;

		glm::vec2 m_SceneSize {};

		EditorCamera m_EditorCamera;
		glm::vec4 m_ViewportBounds{};

		float m_AspectRatio = static_cast<float>(getWindow().getWidth())
				      / static_cast<float>(getWindow().getHeight());


		std::string m_Workspace;


		std::random_device rd;
		std::mt19937 random{rd()};
	};
}
