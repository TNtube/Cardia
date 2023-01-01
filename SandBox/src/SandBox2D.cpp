#include "SandBox2D.hpp"

#include <Cardia.hpp>
#include <imgui.h>


SandBox2D::SandBox2D()
{
	m_TextureSquare = Cardia::Texture2D::create("resources/square.jpg");
	m_TextureBox = Cardia::Texture2D::create("resources/container.jpg");

	std::uniform_int_distribution<int> dist{-7, 6};
	applePos = {dist(random), dist(random), 0.0f};
}

void SandBox2D::OnUpdate()
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

	time += Cardia::Time::deltaTime();
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

	Cardia::RenderAPI::get().setClearColor({0.2f, 0.2f, 0.2f, 1});
	Cardia::RenderAPI::get().clear();

	Cardia::Renderer2D::beginScene(m_Camera, m_Camera.getPosition());

	for (int i = -9; i < 10; ++i)
	{
		for (int j = -9; j < 10; ++j)
		{
			auto x = static_cast<float>(i), y = static_cast<float>(j);
			if (std::find(snakePos.begin(), snakePos.end(), glm::vec3(i, j, 0.0f)) != snakePos.end())
				Cardia::Renderer2D::drawRect({x, y, 0.0f}, {0.95f, 0.95f}, m_TextureSquare.get(), 2);
			else if (applePos == glm::vec3(i, j, 0.0f))
				Cardia::Renderer2D::drawRect({x, y, 0.0f}, {0.95f, 0.95f}, 45,m_TextureBox.get(), {0.8f, 0.3f, 0.2f, 0.5f});
			else
				Cardia::Renderer2D::drawRect({x, y, 0.0f}, {0.95f, 0.95f},{(x + 10.0f) / 20.0f, (y + 10.0f) / 20.0f, 0.3f, 0.8f});
		}
	}

	Cardia::Renderer2D::endScene();
}

void SandBox2D::OnImGuiDraw()
{
	enum ImGuiTheme
	{
		THEME_DARK,
		THEME_LIGHT,
		THEME_CLASSIC
	};
	// fps
	static float elapsedTime = 0.0f;
	static auto fps = static_cast<int>(1000 / Cardia::Time::deltaTime().milliseconds());
	// wireframe
	static bool isWireframeMode = false;
	// fullscreen
	static bool isFullscreen = false;
	static bool isFullscreenPrev = false;
	static Cardia::Window &window = getWindow();
	// vsync
	static bool isVsync = window.isVSync();
	// dear imgui theme
	static int selectedTheme = THEME_DARK;

	ImGui::Begin("Debug tools");

	// Section: Rendering
	if (ImGui::CollapsingHeader("Rendering", ImGuiTreeNodeFlags_DefaultOpen))
	{
		// Section: Rendering > Infos
		ImGui::SetNextItemOpen(true);
		if (ImGui::TreeNode("Infos"))
		{
			ImGui::LabelText(std::to_string(fps).c_str(), "FPS");
			if (elapsedTime >= 0.5f)
			{
				fps = static_cast<int>(1000 / Cardia::Time::deltaTime().milliseconds());
				elapsedTime = 0.0f;
			}
			elapsedTime += Cardia::Time::deltaTime();


			ImGui::LabelText(std::to_string(Cardia::Renderer2D::getStats().drawCalls).c_str(),
					 "Draw Calls");
			ImGui::LabelText(std::to_string(Cardia::Renderer2D::getStats().triangleCount).c_str(),
					 "Triangle Count");
			ImGui::TreePop();
		}

		// Section: Rendering > Options
		ImGui::SetNextItemOpen(true);
		if (ImGui::TreeNode("Options"))
		{
			ImGui::Checkbox("Wireframe rendering?", &isWireframeMode);
			Cardia::RenderAPI::get().setWireFrame(isWireframeMode);

			ImGui::Checkbox("Fullscreen?", &isFullscreen);
			if (isFullscreen != isFullscreenPrev)
			{
				window.setFullscreen(isFullscreen);
				isFullscreenPrev = isFullscreen;
			}

			ImGui::Checkbox("VSync?", &isVsync);
			window.setVSync(isVsync);
			ImGui::TreePop();
		}
	}

	// Section: Fun
	if (ImGui::CollapsingHeader("Fun", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Text("Dear ImGui theme");
		ImGui::RadioButton("Dark", &selectedTheme, THEME_DARK);
		ImGui::SameLine();
		ImGui::RadioButton("Light", &selectedTheme, THEME_LIGHT);
		ImGui::SameLine();
		ImGui::RadioButton("Classic", &selectedTheme, THEME_CLASSIC);
		switch (selectedTheme)
		{
			case THEME_DARK:
				ImGui::StyleColorsDark();
				break;
			case THEME_LIGHT:
				ImGui::StyleColorsLight();
				break;
			case THEME_CLASSIC:
				ImGui::StyleColorsClassic();
				break;
			default:
				break;
		}
	}

	ImGui::End();
}

void SandBox2D::OnEvent(Cardia::Event &event)
{
	Cardia::EventDispatcher dispatcher(event);
	dispatcher.dispatch<Cardia::WindowResizeEvent>(CD_BIND_EVENT_FN(onResize));
}

void SandBox2D::onResize(const Cardia::WindowResizeEvent &e)
{
	m_AspectRatio = static_cast<float>(e.getW()) / static_cast<float>(e.getH());
	m_Camera.setBounds(-m_AspectRatio * m_Zoom, m_AspectRatio * m_Zoom, -m_Zoom, m_Zoom);
}
