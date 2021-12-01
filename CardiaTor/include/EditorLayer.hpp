#pragma once

#include "Panels/SceneHierarchy.hpp"

#include <Cardia.hpp>
#include <random>


namespace Cardia
{
	class EditorLayer : public Layer
	{
	public:
		explicit EditorLayer(std::string name) : Layer(std::move(name)) {}

		void onPush() override;
		void onUpdate(DeltaTime deltaTime) override;
		void onEvent(Event& event) override;
		void onImGuiDraw(DeltaTime deltaTime) override;

	private:
		std::unique_ptr<Texture2D> m_TextureSquare;
		std::unique_ptr<Texture2D> m_TextureBox;
		std::unique_ptr<Framebuffer> m_Framebuffer;

		std::unique_ptr<Scene> m_CurrentScene;
		std::unique_ptr<Panel::SceneHierarchy> m_SceneHierarchyPanel;

		glm::vec2 m_SceneSize {};

		EditorCamera m_EditorCamera;
		glm::vec4 m_ViewportBounds{};

		float m_AspectRatio = static_cast<float>(Application::get().getWindow().getWidth())
				      / static_cast<float>(Application::get().getWindow().getHeight());


		std::random_device rd;
		std::mt19937 random{rd()};
	};
}