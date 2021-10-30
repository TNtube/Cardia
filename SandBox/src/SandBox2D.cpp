#include "SandBox2D.hpp"
#include <Cardia.hpp>


void SandBox2D::onPush()
{
	m_TextureSquare = Cardia::Texture2D::create("assets/square.jpg");
	m_TextureBox = Cardia::Texture2D::create("assets/container.jpg");

	std::uniform_int_distribution<int> dist{-7, 6};
	applePos = {dist(random), dist(random), 0.0f};
	glm::vec3 position = m_Camera.getPosition();

	Cardia::FramebufferSpec spec {};
	auto& window = Cardia::Application::get().getWindow();

	spec.width = window.getWidth();
	spec.height = window.getHeight();

	m_Framebuffer = Cardia::Framebuffer::create(spec);
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

	m_Framebuffer->bind();
	Cardia::RenderCommand::setClearColor({0.2f, 0.2f, 0.2f, 1});
	Cardia::RenderCommand::clear();

	Cardia::Renderer2D::beginScene(m_Camera);

	for (int i = -9; i < 10; ++i)
	{
		for (int j = -9; j < 10; ++j)
		{
			auto x = static_cast<float>(i), y = static_cast<float>(j);
			if (std::find(snakePos.begin(), snakePos.end(), glm::vec3(i, j, 0.0f)) != snakePos.end())
				Cardia::Renderer2D::drawRect({x, y, 0.0f}, {0.95f, 0.95f}, m_TextureSquare.get(), 2);
			else if (applePos == glm::vec3(i, j, 0.0f))
				Cardia::Renderer2D::drawRect({x, y, 0.0f}, {0.95f, 0.95f}, 45,m_TextureBox.get(), {0.8f, 0.3f, 0.2f, 0.0f});
			else
				Cardia::Renderer2D::drawRect({x, y, 0.0f}, {0.95f, 0.95f},{(x + 10.0f) / 20.0f, (y + 10.0f) / 20.0f, 0.3f, 0.8f});
		}
	}

	Cardia::Renderer2D::endScene();
}

void SandBox2D::onImGuiDraw(Cardia::DeltaTime deltaTime)
{
	ImGui::Begin("Game");
	uint32_t textureID = m_Framebuffer->getColorAttachmentRendererID();

	ImVec2 scenePanelSize = ImGui::GetContentRegionAvail();
	if (m_SceneSize != glm::vec2(scenePanelSize.x, scenePanelSize.y))
	{
		m_Framebuffer->resize((int)scenePanelSize.x, (int)scenePanelSize.y);
		m_SceneSize = { scenePanelSize.x, scenePanelSize.y };
	}

	ImGui::Image((void *)textureID, ImVec2{m_SceneSize.x, m_SceneSize.y}, ImVec2{0, 1}, ImVec2{1, 0});
	m_AspectRatio = m_SceneSize.x / m_SceneSize.y;
	m_Camera.setBounds(-m_AspectRatio * m_Zoom, m_AspectRatio * m_Zoom, -m_Zoom, m_Zoom);

	ImGui::End();
}

void SandBox2D::onEvent(Cardia::Event &event)
{
	Cardia::EventDispatcher dispatcher(event);
	dispatcher.dispatch<Cardia::WinResizeEvent>(CD_BIND_EVENT_FN(SandBox2D::onResize));
}

bool SandBox2D::onResize(const Cardia::WinResizeEvent &e)
{
	// m_AspectRatio = static_cast<float>(e.getW()) / static_cast<float>(e.getH());
	// m_Camera.setBounds(-m_AspectRatio * m_Zoom, m_AspectRatio * m_Zoom, -m_Zoom, m_Zoom);
	return false;
}
