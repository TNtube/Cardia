#include "SandBox2D.hpp"
#include <Cardia.hpp>


SandBox2D::SandBox2D(std::string name)
	: Layer(std::move(name))
{
	m_TextureSquare = Cardia::Texture2D::create("assets/container.jpg");
	m_TextureBox = Cardia::Texture2D::create("assets/square.jpg");

	std::uniform_int_distribution<int> dist{-7, 6};
	applePos = {dist(random), dist(random), 0.0f};
}

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
			std::uniform_int_distribution<int> dist{-7, 6};
			applePos = {dist(random), dist(random), 0.0f};
		}
		else
			snakePos.pop_back();
		snakePos.emplace_front(head.x + vx, head.y + vy, head.z);
	}


	Cardia::RenderCommand::setClearColor({0.2f, 0.2f, 0.2f, 1});
	Cardia::RenderCommand::clear();

	Cardia::Renderer2D::beginScene(m_Camera);

	for (int i = -7; i < 7; ++i)
	{
		for (int j = -7; j < 7; ++j)
		{
			auto x = static_cast<float>(i) * 1, y = static_cast<float>(j) * 1;
			if (std::find(snakePos.begin(), snakePos.end(), glm::vec3(i, j, 0.0f)) != snakePos.end())
				Cardia::Renderer2D::drawRect({x, y, 0.0f}, {0.95f, 0.95f}, m_TextureBox.get());
			else if (applePos == glm::vec3(i, j, 0.0f))
				Cardia::Renderer2D::drawRect({x, y, 0.0f}, {0.95f, 0.95f}, m_TextureSquare.get(), {0.8f, 0.3f, 0.2f, 1.0f});
			else
				Cardia::Renderer2D::drawRect({x, y, 0.0f}, {0.95f, 0.95f}, bgColor);
		}
	}

	Cardia::Renderer2D::endScene();
}


void SandBox2D::onImGuiDraw(Cardia::DeltaTime deltaTime)
{
}

void SandBox2D::onEvent(Cardia::Event &event)
{
	Cardia::EventDispatcher dispatcher(event);
	dispatcher.dispatch<Cardia::WinResizeEvent>(CD_BIND_EVENT_FN(SandBox2D::onResize));
}

bool SandBox2D::onResize(const Cardia::WinResizeEvent &e)
{
	m_AspectRatio = static_cast<float>(e.getW()) / static_cast<float>(e.getH());
	m_Camera.setBounds(-m_AspectRatio * m_Zoom, m_AspectRatio * m_Zoom, -m_Zoom, m_Zoom);
	return false;
}


