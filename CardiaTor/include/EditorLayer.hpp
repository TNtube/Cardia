#pragma once
#include <Cardia.hpp>
#include <imgui.h>
#include <deque>
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

		glm::vec2 m_SceneSize {};

		float time = 0.0f;

		float m_Zoom = 10.0f;
		float m_AspectRatio = static_cast<float>(Application::get().getWindow().getWidth())
				      / static_cast<float>(Application::get().getWindow().getHeight());
		OrthographicCamera m_Camera {-m_AspectRatio * m_Zoom, m_AspectRatio * m_Zoom, -m_Zoom, m_Zoom};


		std::random_device rd;
		std::mt19937 random{rd()};
	};
}