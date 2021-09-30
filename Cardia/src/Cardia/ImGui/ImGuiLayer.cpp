#include "cdpch.hpp"
#include "Cardia/ImGui/ImGuiLayer.hpp"

#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>

#include "Cardia/Application.hpp"

#include <GLFW/glfw3.h>

namespace Cardia
{
	ImGuiLayer::ImGuiLayer()
		: Layer("ImGuiLayer")
	{

	}

	void ImGuiLayer::onPush()
	{
		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		ImGui::StyleColorsDark();

		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
		    style.WindowRounding = 0.0f;
		    style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		Application& app = Application::get();
		auto window = static_cast<GLFWwindow*>(app.getWindow().getNativeWin());
		ImGui_ImplGlfw_InitForOpenGL(window, true);

		ImGui_ImplOpenGL3_Init("#version 410");
	}

	void ImGuiLayer::onPop()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiLayer::Begin()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void ImGuiLayer::End()
	{
		ImGuiIO& io = ImGui::GetIO();
		Application& app = Application::get();
		auto[w, h] = app.getWindow().getSize();
		io.DisplaySize = ImVec2(static_cast<float>(w), static_cast<float>(h));

		// Rendering
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
	}

	static void DebugWindow(DeltaTime deltaTime)
	{
		enum ImGuiTheme {
			THEME_DARK,
			THEME_LIGHT,
			THEME_CLASSIC
		};
		// wireframe
		static bool isWireframeMode = false;
		// fullscreen
		static bool isFullscreen = false;
		static bool isFullscreenPrev = false;
		static Window& window = Application::get().getWindow();
		// fps
		static uint64_t fpsDeltaCounter = 0; // deltaTime since last exec
		static uint16_t frames = 0;
		static auto fps = static_cast<int>(1000 / deltaTime.milliseconds());
		static auto fpsClock = std::chrono::high_resolution_clock::now();
		static auto fpsClock2 = std::chrono::high_resolution_clock::now();
		// dear imgui theme
		static int selectedTheme = THEME_DARK;

		ImGui::Begin("Debug tools");

		ImGui::Checkbox("Wireframe rendering?", &isWireframeMode);
		RenderCommand::setWireFrame(isWireframeMode);

		ImGui::Checkbox("Fullscreen?", &isFullscreen);
		if (isFullscreen != isFullscreenPrev)
		{
			window.setFullscreen(isFullscreen);
			isFullscreenPrev = isFullscreen;
		}

		ImGui::LabelText(std::to_string(fps).c_str(), "FPS");
		fpsClock2 = std::chrono::high_resolution_clock::now();
		fpsDeltaCounter += deltaTime.milliseconds();
		++frames;
		int cnt = std::chrono::duration_cast<std::chrono::milliseconds>(fpsClock2 - fpsClock).count();
		if (cnt >= 1000)
		{
			fps = cnt / (fpsDeltaCounter / frames);
			fpsDeltaCounter = 0;
			frames = 0;
			fpsClock = std::chrono::high_resolution_clock::now();
			fpsClock2 = fpsClock;
		}

		if (ImGui::CollapsingHeader("Fun", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Text("Dear ImGui theme");
			ImGui::RadioButton("Dark", &selectedTheme, THEME_DARK); ImGui::SameLine();
			ImGui::RadioButton("Light", &selectedTheme, THEME_LIGHT); ImGui::SameLine();
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
			}
		}

		ImGui::End();
	}

	void ImGuiLayer::onImGuiDraw(DeltaTime deltaTime)
	{
		DebugWindow(deltaTime);
	}
}
