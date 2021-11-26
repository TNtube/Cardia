#include "EditorLayer.hpp"
#include <Cardia.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include <ImGuizmo.h>

namespace Cardia
{
	void EditorLayer::onPush()
	{

		FramebufferSpec spec{};
		auto &window = Application::get().getWindow();

		spec.width = window.getWidth();
		spec.height = window.getHeight();

		m_CurrentScene = std::make_unique<Scene>("Default Scene");
		m_SceneHierarchyPanel = std::make_unique<Panel::SceneHierarchy>(m_CurrentScene.get());

		auto component = m_CurrentScene->createEntity("Blue Square");
		component.addComponent<Component::SpriteRenderer>(glm::vec4{0.2f, 0.8f, 0.8f, 1.0f});

		component = m_CurrentScene->createEntity("Red Square");
		component.addComponent<Component::SpriteRenderer>(glm::vec4{0.8f, 0.2f, 0.3f, 1.0f});

		component = m_CurrentScene->createEntity("Camera");
		component.addComponent<Component::Camera>();

		m_Framebuffer = Framebuffer::create(spec);
	}


	void EditorLayer::onUpdate(DeltaTime deltaTime)
	{
		m_Framebuffer->bind();
		RenderCommand::setClearColor({0.2f, 0.2f, 0.2f, 1});
		RenderCommand::clear();

		m_EditorCamera.onUpdate(deltaTime);
		m_CurrentScene->onUpdateEditor(deltaTime, m_EditorCamera);

		m_Framebuffer->unbind();
	}

	static void EnableDocking()
	{
		// Note: Switch this to true to enable dockspace
		static bool dockingEnabled = true;
		if (dockingEnabled)
		{
			static bool dockspaceOpen = true;
			static bool opt_fullscreen_persistant = true;
			bool opt_fullscreen = opt_fullscreen_persistant;
			static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

			// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
			// because it would be confusing to have two docking targets within each others.
			ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
			if (opt_fullscreen)
			{
				ImGuiViewport *viewport = ImGui::GetMainViewport();
				ImGui::SetNextWindowPos(viewport->Pos);
				ImGui::SetNextWindowSize(viewport->Size);
				ImGui::SetNextWindowViewport(viewport->ID);
				ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
				ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
				window_flags |=
					ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
					ImGuiWindowFlags_NoResize |
					ImGuiWindowFlags_NoMove;
				window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
			}

			// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
			if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
				window_flags |= ImGuiWindowFlags_NoBackground;

			// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
			// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
			// all active windows docked into it will lose their parent and become undocked.
			// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
			// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
			ImGui::Begin("DockSpace", &dockspaceOpen, window_flags);
			ImGui::PopStyleVar();

			if (opt_fullscreen)
				ImGui::PopStyleVar(2);

			// DockSpace
			ImGuiIO &io = ImGui::GetIO();
			if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
			{
				ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
				ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
			}

			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("File"))
				{
					// Disabling fullscreen would allow the window to be moved to the front of other windows,
					// which we can't undo at the moment without finer window depth/z control.
					//ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);

					if (ImGui::MenuItem("Exit"))
					{
						Application::get().close();
					}
					ImGui::EndMenu();
				}

				ImGui::EndMenuBar();
			}
			ImGui::End();
		}
	}

	static void DebugWindow(DeltaTime deltaTime) {
		enum ImGuiTheme
		{
			THEME_DARK,
			THEME_LIGHT,
			THEME_CLASSIC
		};
		// fps
		static float elapsedTime = 0.0f;
		static auto fps = static_cast<int>(1000 / deltaTime.milliseconds());
		// wireframe
		static bool isWireframeMode = false;
		// fullscreen
		static bool isFullscreen = false;
		static bool isFullscreenPrev = false;
		static Window &window = Application::get().getWindow();
		// vsync
		static bool isVsync = window.isVSync();
		// dear imgui theme
		static int selectedTheme = THEME_DARK;

		ImGui::Begin("Debug tools");

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
					fps = static_cast<int>(1000 / deltaTime.milliseconds());
					elapsedTime = 0.0f;
				}
				elapsedTime += deltaTime.seconds();


				ImGui::LabelText(
					std::to_string(Renderer2D::getStats().drawCalls).c_str(),
					"Draw Calls");
				ImGui::LabelText(
					std::to_string(Renderer2D::getStats().triangleCount).c_str(),
					"Triangle Count");
				ImGui::Separator();
				ImGui::Text("GPU's Info");
				ImGui::Text("Vendor   : %s", RenderCommand::getVendor().c_str());
				ImGui::Text("Renderer : %s", RenderCommand::getRenderer().c_str());
				ImGui::Text("Version  : %s", RenderCommand::getVersion().c_str());
				ImGui::Separator();
				ImGui::TreePop();
			}

			// Section: Rendering > Options
			isOpen = ImGui::TreeNodeEx("Options", ImGuiTreeNodeFlags_DefaultOpen);
			if (isOpen)
			{
				ImGui::Checkbox("Wireframe rendering?", &isWireframeMode);
				RenderCommand::setWireFrame(isWireframeMode);

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

	void EditorLayer::onImGuiDraw(DeltaTime deltaTime)
	{
		EnableDocking();
		DebugWindow(deltaTime);
		m_SceneHierarchyPanel->onImGuiRender(deltaTime);

		ImGui::Begin("Edit");

		auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
		auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
		auto viewportOffset = ImGui::GetWindowPos();
		m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
		m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };
		uint32_t textureID = m_Framebuffer->getColorAttachmentRendererID();

		ImVec2 scenePanelSize = ImGui::GetContentRegionAvail();
		if (m_SceneSize != glm::vec2(scenePanelSize.x, scenePanelSize.y))
		{
			m_Framebuffer->resize(static_cast<int>(scenePanelSize.x), static_cast<int>(scenePanelSize.y));
			m_SceneSize = {scenePanelSize.x, scenePanelSize.y};
		}

		ImGui::Image(reinterpret_cast<void *>(static_cast<size_t>(textureID)),
			     ImVec2{m_SceneSize.x, m_SceneSize.y},
			     ImVec2{0, 1}, ImVec2{1, 0});

		m_AspectRatio = m_SceneSize.x / m_SceneSize.y;

		m_EditorCamera.setViewportSize(m_SceneSize.x, m_SceneSize.y);

		ImGuizmo::SetRect(0, 0, m_SceneSize.x, m_SceneSize.y);
		auto selectedEntity = m_SceneHierarchyPanel->getClickedEntity();
		if (selectedEntity)
		{
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();

			ImGuizmo::SetRect(m_ViewportBounds[0].x, m_ViewportBounds[0].y, m_ViewportBounds[1].x - m_ViewportBounds[0].x, m_ViewportBounds[1].y - m_ViewportBounds[0].y);

			// Editor camera
			const glm::mat4& cameraProjection = m_EditorCamera.getProjection();
			glm::mat4 cameraView = m_EditorCamera.getViewMatrix();
			auto& transformComponent = selectedEntity.getComponent<Component::Transform>();
			glm::mat4 transform = transformComponent.getTransform();

			ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
					     ImGuizmo::TRANSLATE, ImGuizmo::LOCAL, glm::value_ptr(transform),
					     nullptr, nullptr);

			if (ImGuizmo::IsUsing())
			{
				glm::vec3 translation, scale, skew;
				glm::quat rotation;
				glm::vec4 perspective;
				glm::decompose(transform, scale, rotation, translation, skew, perspective);
				transformComponent.rotation = glm::eulerAngles(rotation);
				transformComponent.position = translation;
				transformComponent.scale = scale;

			}
		}

		ImGui::End();
	}

	void EditorLayer::onEvent(Event &event)
	{
		m_EditorCamera.onEvent(event);
	}
}