#pragma once
#include <Cardia.hpp>
#include <imgui.h>
#include <deque>
#include <random>


class SandBox2D : public Cardia::Layer
{
public:
	explicit SandBox2D(std::string name);

	void onUpdate(Cardia::DeltaTime deltaTime) override;
	void onEvent(Cardia::Event& event) override;
	void onImGuiDraw(Cardia::DeltaTime deltaTime) override;

private:
	std::unique_ptr<Cardia::Texture2D> m_TextureSquare;
	std::unique_ptr<Cardia::Texture2D> m_TextureBox;

	glm::vec4 bgColor {0.3f, 0.2f, 0.8f, 1.0f};
	glm::vec4 snakeCol {0.3f, 0.8f, 0.2f, 0.1f};

	std::deque<glm::vec3> snakePos {{0.0f, 0.0f, 0.0f},
					{1.0f, 0.0f, 0.0f},
					{1.0f, 0.0f, 0.0f}};

	glm::vec3 applePos {0.0f};

	float vx = 1, vy = 0;
	float time = 0.0f;

	Cardia::OrthographicCamera m_Camera {0, 1080, 0, 720};


	std::random_device rd;
	std::mt19937 random{rd()};
};