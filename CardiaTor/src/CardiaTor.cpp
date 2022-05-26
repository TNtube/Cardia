#include "CardiaTor.hpp"

#include <Cardia.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <imgui.h>
#include <ImGuizmo.h>
#include <nfd.h>
#include <Cardia/SerializationUtils/SceneSerializer.hpp>


namespace Cardia
{
	CardiaTor::CardiaTor()
	{
		const auto &window = getWindow();

		const FramebufferSpec spec{ window.getWidth(), window.getHeight() };

		m_CurrentScene = std::make_unique<Scene>("Default Scene");
		m_SceneHierarchyPanel = std::make_unique<Panel::SceneHierarchy>(m_CurrentScene.get());
		m_InspectorPanel = std::make_unique<Panel::InspectorPanel>(m_SceneHierarchyPanel->getClickedEntity());
		m_DebugPanel = std::make_unique<Panel::DebugPanel>();
		m_FileHierarchyPanel = std::make_unique<Panel::FileHierarchy>(m_Workspace);

		Entity entity;
		for (int x = 0; x < 9; x++)
		{
			for (int y = 0; y < 9; y++)
			{
				entity = m_CurrentScene->createEntity("Square-" + std::to_string(x) + "-" + std::to_string(y));
				entity.getComponent<Component::Transform>().position = {x, y, 0};
				entity.addComponent<Component::SpriteRenderer>(glm::vec4{static_cast<float>(x)/10.0f, static_cast<float>(y)/10.0f, static_cast<float>(x + y)/20.0f, 1.0f});

			}
		}

		entity = m_CurrentScene->createEntity("Camera");
		entity.addComponent<Component::Camera>();

		Log::coreInfo(SerializerUtils::SerializeScene(m_CurrentScene.get()));

		m_Framebuffer = Framebuffer::create(spec);
	}


	void CardiaTor::onUpdate(DeltaTime deltaTime)
	{
		m_Framebuffer->bind();
		RenderAPI::get().setClearColor({0.2f, 0.2f, 0.2f, 1});
		RenderAPI::get().clear();

		m_EditorCamera.onUpdate(deltaTime);
		m_CurrentScene->onUpdateEditor(deltaTime, m_EditorCamera);

		m_Framebuffer->unbind();
	}

	void CardiaTor::enableDocking()
	{
		// Note: Switch this to true to enable dockspace
		static bool dockingEnabled = true;
		if (dockingEnabled)
		{
			static bool dockspaceOpen = true;
			static bool opt_fullscreen_persistant = true;
			const bool opt_fullscreen = opt_fullscreen_persistant;
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
					if (ImGui::MenuItem("Open"))
					{
						char *outPath = nullptr;
						nfdresult_t result = NFD_PickFolder( nullptr, &outPath );
        
						if ( result == NFD_OKAY ) {
							m_Workspace = std::string(outPath);
							m_FileHierarchyPanel->updateWorkspace(m_Workspace);
							Log::coreInfo("Workspace : {0}", m_Workspace);
						}
					}

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

	void CardiaTor::onImGuiDraw(DeltaTime deltaTime)
	{
		enableDocking();
		m_DebugPanel->onImGuiRender(deltaTime);
		m_InspectorPanel->updateSelectedEntity(m_SceneHierarchyPanel->getClickedEntity());
		m_SceneHierarchyPanel->onImGuiRender(deltaTime);
		m_InspectorPanel->onImGuiRender(deltaTime);
		m_FileHierarchyPanel->onImGuiRender(deltaTime);

		ImGui::Begin("Edit");

		m_HoverViewport = ImGui::IsWindowHovered();

		const auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
		const auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
		const auto viewportOffset = ImGui::GetWindowPos();
		m_ViewportBounds = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y,
			viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };
		const uint32_t textureID = m_Framebuffer->getColorAttachmentRendererID();

		ImVec2 scenePanelSize = ImGui::GetContentRegionAvail();
		if (m_SceneSize != glm::vec2(scenePanelSize.x, scenePanelSize.y))
		{
			m_Framebuffer->resize(static_cast<int>(scenePanelSize.x), static_cast<int>(scenePanelSize.y));
			m_SceneSize = {scenePanelSize.x, scenePanelSize.y};
		}

		ImGui::Image(reinterpret_cast<ImTextureID>(static_cast<size_t>(textureID)),
			     ImVec2{m_SceneSize.x, m_SceneSize.y},
			     ImVec2{0, 1}, ImVec2{1, 0});

		m_AspectRatio = m_SceneSize.x / m_SceneSize.y;

		m_EditorCamera.setViewportSize(m_SceneSize.x, m_SceneSize.y);

		auto selectedEntity = m_SceneHierarchyPanel->getClickedEntity();
		if (selectedEntity)
		{
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();

			ImGuizmo::SetRect(m_ViewportBounds.x, m_ViewportBounds.y, m_SceneSize.x, m_SceneSize.y);

			// Editor camera
			const glm::mat4& cameraProjection = m_EditorCamera.getProjectionMatrix();
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

	void CardiaTor::onEvent(Event& event)
	{
		const auto [x, y] = Input::getMousePos();
		if (m_HoverViewport)
		{
			// Mouse inside viewport
			m_EditorCamera.onEvent(event);
		}
	}
}
