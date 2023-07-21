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

	glm::vec2 m_SceneSize {};

	glm::vec4 bgColor {0.3f, 0.2f, 0.8f, 1.0f};
	glm::vec4 snakeCol {0.3f, 0.8f, 0.2f, 0.1f};

	std::deque<glm::vec3> snakePos {{0.0f, 0.0f, 0.0f},
					{1.0f, 0.0f, 0.0f},
					{1.0f, 0.0f, 0.0f}};

	glm::vec3 applePos {0.0f};

	float vx = 1, vy = 0;
	float time = 0.0f;

	float m_Zoom = 10.0f;
	float m_AspectRatio = static_cast<float>(get().GetWindow().getWidth())
		/ static_cast<float>(get().GetWindow().getHeight());


	std::random_device rd;
	std::mt19937 random{rd()};
};