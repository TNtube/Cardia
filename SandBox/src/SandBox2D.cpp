#include "SandBox2D.hpp"

#include <Cardia.hpp>
#include <imgui.h>


SandBox2D::SandBox2D()
{
	// m_TextureSquare = Cardia::Texture2D::create("resources/square.jpg");
	// m_TextureBox = Cardia::Texture2D::create("resources/container.jpg");

	std::uniform_int_distribution<int> dist{-7, 6};
	applePos = {dist(random), dist(random), 0.0f};
}

void SandBox2D::OnUpdate()
{
	if (Cardia::Input::IsKeyPressed(Cardia::Key::Left)) {
		vx = -1;
		vy = 0;
	}
	else if (Cardia::Input::IsKeyPressed(Cardia::Key::Right)) {
		vx = 1;
		vy = 0;
	}
	if (Cardia::Input::IsKeyPressed(Cardia::Key::Down)) {
		vx = 0;
		vy = -1;
	}
	if (Cardia::Input::IsKeyPressed(Cardia::Key::Up)) {
		vx = 0;
		vy = 1;
	}

	time += Cardia::Time::GetDeltaTime();
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

	// Cardia::RenderAPI::get().setClearColor({0.2f, 0.2f, 0.2f, 1});
	// Cardia::RenderAPI::get().clear();
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
	static auto fps = static_cast<int>(1000 / Cardia::Time::GetDeltaTime().AsMilliseconds());
	// wireframe
	static bool isWireframeMode = false;
	// fullscreen
	static bool isFullscreen = false;
	static bool isFullscreenPrev = false;
	static Cardia::Window &window = GetWindow();
	// vsync
	static bool isVsync = window.IsVSync();
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
				fps = static_cast<int>(1000 / Cardia::Time::GetDeltaTime().AsMilliseconds());
				elapsedTime = 0.0f;
			}
			elapsedTime += Cardia::Time::GetDeltaTime();


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
			// Cardia::RenderAPI::get().setWireFrame(isWireframeMode);

			ImGui::Checkbox("Fullscreen?", &isFullscreen);
			if (isFullscreen != isFullscreenPrev)
			{
				window.SetFullscreen(isFullscreen);
				isFullscreenPrev = isFullscreen;
			}

			ImGui::Checkbox("VSync?", &isVsync);
			window.SetVSync(isVsync);
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
}
