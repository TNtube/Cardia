#include "Panels/DebugPanel.hpp"

#include <imgui.h>

#include "Cardia/Application.hpp"
#include "Cardia/Core/Window.hpp"
#include "Cardia/Renderer/RenderAPI.hpp"
#include "Cardia/Renderer/Renderer2D.hpp"
#include "Panels/PanelManager.hpp"


namespace Cardia::Panel
{
	int DebugPanel::m_LastWindowId = 0;
	void DebugPanel::OnImGuiRender(CardiaTor* appContext)
	{
		enum ImGuiTheme
		{
			THEME_DARK,
			THEME_LIGHT,
			THEME_CLASSIC
		};
		// fps
		static float elapsedTime = 0.0f;
		static auto fps = static_cast<int>(1000 / Time::deltaTime().milliseconds());
		// wireframe
		static bool isWireframeMode = false;
		// fullscreen
		static bool isFullscreen = false;
		static bool isFullscreenPrev = false;
		static Window &window = Application::get().GetWindow();
		// vsync
		static bool isVsync = window.isVSync();
		// dear imgui theme
		static int selectedTheme = THEME_DARK;

		char buff[64];
		sprintf(buff, "Debug tools##%i", m_WindowId);
		ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);
		if (!ImGui::Begin(buff, &m_IsOpen)) {
			ImGui::End();
			return;
		}

		if (ImGui::IsWindowFocused()) {
			m_PanelManager->SetFocused<DebugPanel>(this);
		}

		// Section: Rendering
		if (ImGui::CollapsingHeader("Rendering", ImGuiTreeNodeFlags_DefaultOpen))
		{
			// Section: Rendering > Infos
			static bool isOpen;
			isOpen = ImGui::TreeNodeEx("Infos", ImGuiTreeNodeFlags_DefaultOpen);
			if (isOpen)
			{
				ImGui::LabelText(std::to_string(fps).c_str(), "FPS");
				if (elapsedTime >= 0.5f)
				{
					fps = static_cast<int>(1000 / Time::deltaTime().milliseconds());
					elapsedTime = 0.0f;
				}
				elapsedTime += Time::deltaTime().seconds();


				ImGui::LabelText(
					std::to_string(Renderer2D::getStats().drawCalls).c_str(),
					"Draw Calls");
				ImGui::LabelText(
					std::to_string(Renderer2D::getStats().triangleCount).c_str(),
					"Triangle Count");
				ImGui::Separator();
				ImGui::Text("GPU's Info");
				ImGui::Text("Vendor   : %s", RenderAPI::get().getVendor().c_str());
				ImGui::Text("Renderer : %s", RenderAPI::get().getRenderer().c_str());
				ImGui::Text("Version  : %s", RenderAPI::get().getVersion().c_str());
				ImGui::Separator();
				ImGui::TreePop();
			}

			// Section: Rendering > Options
			isOpen = ImGui::TreeNodeEx("Options", ImGuiTreeNodeFlags_DefaultOpen);
			if (isOpen)
			{
				ImGui::Checkbox("Wireframe rendering?", &isWireframeMode);
				RenderAPI::get().setWireFrame(isWireframeMode);

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
}
