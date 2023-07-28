#pragma once

#include <Cardia.hpp>
#include <deque>
#include <random>


class SandBox2D : public Cardia::Application
{
public:
	SandBox2D();

	void OnUpdate() override;
	void OnEvent(Cardia::Event& event) override;
	void OnRender() override {}
	void OnImGuiDraw() override;
	Cardia::Scene* GetCurrentScene() override{ return nullptr; }

private:
	void onResize(const Cardia::WindowResizeEvent& e);
	std::unique_ptr<Cardia::Texture2D> m_TextureSquare;
	std::unique_ptr<Cardia::Texture2D> m_TextureBox;

	Cardia::Vector2f m_SceneSize {};

	Cardia::Vector4f bgColor {0.3f, 0.2f, 0.8f, 1.0f};
	Cardia::Vector4f snakeCol {0.3f, 0.8f, 0.2f, 0.1f};

	std::deque<Cardia::Vector3f> snakePos {{0.0f, 0.0f, 0.0f},
					{1.0f, 0.0f, 0.0f},
					{1.0f, 0.0f, 0.0f}};

	Cardia::Vector3f applePos {0.0f};

	float vx = 1, vy = 0;
	float time = 0.0f;

	float m_Zoom = 10.0f;
	float m_AspectRatio = static_cast<float>(Get().GetWindow().GetWidth())
		/ static_cast<float>(Get().GetWindow().GetHeight());


	std::random_device rd;
	std::mt19937 random{rd()};
};