#include "utpch.hpp"
#include "ImGuiLayer.hpp"

#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>

#include "Utopia/Application.hpp"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace Utopia
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

	static std::array<int, 2> WinInitPos(GLFWwindow* win)
	{
		int x, y;
		glfwGetWindowPos(win, &x, &y);
		return { x, y };
	}

	static void DebugWindow()
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
		static const Window& window = Application::get().getWindow();
		static const auto glWindow = static_cast<GLFWwindow*>(window.getNativeWin());
		static auto winPos = WinInitPos(glWindow);
		static int winSize[] = { window.getWidth(), window.getHeight() };
		// dear imgui theme
		static int selectedTheme = THEME_DARK;

		ImGui::Begin("Debug tools");

		ImGui::Checkbox("Wireframe rendering?", &isWireframeMode);
		glPolygonMode(GL_FRONT_AND_BACK, isWireframeMode ? GL_LINE : GL_FILL);

		ImGui::Checkbox("Fullscreen?", &isFullscreen);
		if (isFullscreen != isFullscreenPrev)
		{
			if (isFullscreen)
			{
				const auto monitor = glfwGetPrimaryMonitor();
				const auto mode = glfwGetVideoMode(monitor);
				glfwGetWindowPos(glWindow, &winPos[0], &winPos[1]);
				glfwGetWindowSize(glWindow, &winSize[0], &winSize[1]);
				glfwSetWindowMonitor(glWindow, monitor, 0, 0, mode->width, mode->height, window.isVSync() ? mode->refreshRate : 0);
			}
			else
			{
				glfwSetWindowMonitor(glWindow, nullptr, winPos[0], winPos[1], winSize[0], winSize[1], 0);
			}
			int vpWidth, vpHeight;
			glfwGetFramebufferSize(glWindow, &vpWidth, &vpHeight);
			glViewport(0, 0, vpWidth, vpHeight);
			isFullscreenPrev = isFullscreen;
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

	void ImGuiLayer::onImGuiDraw()
	{
		static bool show = true;
		ImGui::ShowDemoWindow(&show);
		DebugWindow();
	}
}
