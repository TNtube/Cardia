#include "SandBox2D.hpp"
#include <Cardia.hpp>


void SandBox2D::onUpdate(Cardia::DeltaTime deltaTime)
{
	if (Cardia::Input::isKeyPressed(Cardia::Key::Left)) {
		vx = -1;
		vy = 0;
	}
	else if (Cardia::Input::isKeyPressed(Cardia::Key::Right)) {
		vx = 1;
		vy = 0;
	}
	if (Cardia::Input::isKeyPressed(Cardia::Key::Down)) {
		vx = 0;
		vy = -1;
	}
	if (Cardia::Input::isKeyPressed(Cardia::Key::Up)) {
		vx = 0;
		vy = 1;
	}

	time += deltaTime.seconds();
	if (time > 0.2f) {
		time = 0.0f;
		glm::vec3 head = snakePos.front();
		if(head == applePos) {
			std::uniform_int_distribution<int> dist{0, 13};
			applePos = {dist(random), dist(random), 0.0f};
		}
		else
			snakePos.pop_back();
		snakePos.emplace_front(head.x + vx, head.y + vy, head.z);
	}


	Cardia::RenderCommand::setClearColor({0.2f, 0.2f, 0.2f, 1});
	Cardia::RenderCommand::clear();

	Cardia::Renderer2D::beginScene(m_Camera);

	for (int i = 0; i < 14; ++i)
	{
		for (int j = 0; j < 14; ++j)
		{
			float x = (float)i * 50 + 10, y = (float)j * 50 + 10;
			if (std::find(snakePos.begin(), snakePos.end(), glm::vec3(i, j, 0.0f)) != snakePos.end())
				Cardia::Renderer2D::drawRect({x, y, 0.0f}, {45.0f, 45.f}, snakeCol);
			else if (applePos == glm::vec3(i, j, 0.0f))
				Cardia::Renderer2D::drawRect({x, y, 0.0f}, {45.0f, 45.f}, {0.8f, 0.3f, 0.2f, 1.0f});
			else
				Cardia::Renderer2D::drawRect({x, y, 0.0f}, {45.0f, 45.f}, bgColor);
		}
	}

	Cardia::Renderer2D::endScene();
}


void SandBox2D::onImGuiDraw(Cardia::DeltaTime deltaTime)
{
}

void SandBox2D::onEvent(Cardia::Event &event)
{
}


