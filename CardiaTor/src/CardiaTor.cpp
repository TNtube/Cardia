#include "CardiaTor.hpp"

#include <Cardia.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <imgui.h>
#include <ImGuizmo.h>
#include <nfd.h>
#include <ranges>
#include <Cardia/Serialization/SceneSerializer.hpp>

#include "Panels/SceneHierarchyPanel.hpp"
#include "Panels/DebugPanel.hpp"
#include "Panels/FileHierarchyPanel.hpp"
#include "Panels/InspectorPanel.hpp"
#include "Cardia/Project/Project.hpp"
#include "Cardia/Project/AssetsManager.hpp"
#include "Panels/ConsolePanel.hpp"


namespace Cardia
{
	CardiaTor::CardiaTor()
	{
		const auto &window = GetWindow();

		m_PanelManager.CreatePanel<Panel::SceneHierarchyPanel>();
		m_PanelManager.CreatePanel<Panel::InspectorPanel>();
		m_PanelManager.CreatePanel<Panel::DebugPanel>();
		m_PanelManager.CreatePanel<Panel::FileHierarchyPanel>();
		m_PanelManager.CreatePanel<Panel::ConsolePanel>();

		m_CurrentScene = std::make_unique<Scene>(m_Renderer);

		for (auto& panel: m_PanelManager.Panels()) {
			panel->OnSceneLoad(m_CurrentScene.get());
		}

		m_IconPlay = std::make_unique<Texture2D>(m_Renderer.GetDevice(), "resources/icons/play.png");
		m_IconStop = std::make_unique<Texture2D>(m_Renderer.GetDevice(), "resources/icons/pause.png");

		// FramebufferSpecification spec{ window.getWidth(), window.getHeight() };
		// spec.attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INTEGER, FramebufferTextureFormat::Depth };
		// m_Framebuffer = Framebuffer::create(spec);

		ImGuiIO &io = ImGui::GetIO();
		io.IniFilename = "resources/editorconfig.ini";

		Logger::AddEditorCallback([&](const spdlog::level::level_enum& level, const std::string& msg) {
			m_PanelManager.GetLastFocused<Panel::ConsolePanel>()->AddLog(level, msg);
		});
	}


	void CardiaTor::OnUpdate()
	{
		// m_Framebuffer->Bind();
		// RenderAPI::get().setClearColor({0.2f, 0.2f, 0.2f, 1});
		// RenderAPI::get().clear();
		// m_Framebuffer->ClearAttachment(1, -1);


		if (m_EditorState == EditorState::Edit)
		{
			m_EditorCamera.OnUpdate();
		} else if (m_EditorState == EditorState::Play)
		{
			ScriptEngine::Instance().OnRuntimeUpdate();
		}

		auto[mx, my] = ImGui::GetMousePos();
		mx -= m_ViewportBounds.x;
		my -= m_ViewportBounds.y;

		glm::vec2 viewportSize = glm::vec2(m_ViewportBounds.z - m_ViewportBounds.x, m_ViewportBounds.w - m_ViewportBounds.y);
		my = viewportSize.y - my;
		int mouseX = (int)mx;
		int mouseY = (int)my;

		if (m_CurrentScene)
		{
			if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)viewportSize.x && mouseY < (int)viewportSize.y)
			{
				// int pixelData = m_Framebuffer->ReadPixel(1, mouseX, mouseY);
				// m_HoveredEntity = pixelData == -1 ? Entity() : Entity((entt::entity)pixelData, m_CurrentScene.get());
			}
		}

		// m_Framebuffer->Unbind();
	}

	void CardiaTor::OnRender(VkCommandBuffer commandBuffer)
	{
		if (m_EditorState == EditorState::Edit)
		{
			m_CurrentScene->OnRender(commandBuffer, m_EditorCamera.GetCamera(), m_EditorCamera.GetTransform());
		} else if (m_EditorState == EditorState::Play)
		{
			m_CurrentScene->OnRuntimeRender(commandBuffer);
		}
	}

	void CardiaTor::EnableDocking()
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
					if (ImGui::MenuItem("Open", "CTRL+O"))
					{
						OpenProject();
					}

					if (ImGui::MenuItem("Exit"))
					{
						Application::get().close();
					}
					ImGui::EndMenu();
				}
				if (ImGui::BeginMenu("Panels"))
				{
					if (ImGui::MenuItem("SceneHierarchy")) {
						auto panel = m_PanelManager.CreatePanel<Panel::SceneHierarchyPanel>();
						panel->OnSceneLoad(m_CurrentScene.get());
					}
					if (ImGui::MenuItem("Inspector")) {
						auto panel = m_PanelManager.CreatePanel<Panel::InspectorPanel>();
						panel->OnSceneLoad(m_CurrentScene.get());
					}
					if (ImGui::MenuItem("Debug")) {
						m_PanelManager.CreatePanel<Panel::DebugPanel>();
					}
					if (ImGui::MenuItem("FileHierarchy")) {
						auto panel = m_PanelManager.CreatePanel<Panel::FileHierarchyPanel>();
						panel->OnSceneLoad(m_CurrentScene.get());
						panel->OnUpdateWorkspace();
					}
					if (ImGui::MenuItem("Terminal")) {
						m_PanelManager.CreatePanel<Panel::ConsolePanel>();
					}
					ImGui::EndMenu();
				}

				ImGui::EndMenuBar();
			}
			ImGui::End();
		}
	}

	void CardiaTor::OpenProject()
	{
		char *outPath = nullptr;
		const nfdresult_t result = NFD_OpenDialog( "cdproj", "", &outPath );

		if ( result == NFD_OKAY ) {
			auto project = Project::Load(outPath);
			if (project) {
				Log::info("OpeningProject : {0}", project->GetConfig().Name);
				InvalidateProject();
			} else {
				Log::warn("Failed To LoadImpl Project : {0}", outPath);
			}
		}
	}

	void CardiaTor::InvalidateProject()
	{
		for (auto& panel : m_PanelManager.Panels())
		{
			panel->OnUpdateWorkspace();
		}
		ScriptEngine::InvalidateProject();
		OpenScene(AssetsManager::GetAssetAbsolutePath(Project::GetActive()->GetConfig().StartScene));
		m_CurrentScene->Init();
	}

	void CardiaTor::InvalidateScene()
	{
		m_HoveredEntity = Entity();
		m_SelectedEntity = Entity();
		for (auto& panel: m_PanelManager.Panels()) {
			panel->OnSceneLoad(m_CurrentScene.get());
		}
	}

	void CardiaTor::SaveScene() const
	{
		auto& path = m_CurrentScene->GetPath();
		if (path.empty())
		{
			nfdchar_t *outPath = nullptr;
			NFD_SaveDialog("cardia", "", &outPath);
			if (!outPath)
			{
				return;
			}
			path = outPath;
			if (path.extension() != ".cardia")
			{
				path += ".cardia";
			}
		}

		Serialization::SceneSerializer serializer(*m_CurrentScene);
		serializer.Serialize(path);
	}

	void CardiaTor::OpenScene(const std::filesystem::path& scenePath)
	{
		auto newScene = std::make_unique<Scene>(m_Renderer, scenePath);
		Serialization::SceneSerializer serializer(*newScene);
		if (!serializer.Deserialize(m_Renderer, scenePath))
		{
			Log::info("Unable to load {0}", scenePath.string());
		}
		else
		{
			m_CurrentScene = std::move(newScene);
		}
		InvalidateScene();
	}

	void CardiaTor::ReloadScene()
	{
		if (m_LastEditorScene) {
			m_CurrentScene = Scene::Copy(*m_LastEditorScene);
			InvalidateScene();
		}
	}

	void CardiaTor::OnImGuiDraw()
	{
		EnableDocking();

		for (const auto& panel : m_PanelManager.Panels())
		{
			if (panel->IsOpen()) panel->OnImGuiRender(this);
		}
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Edit", nullptr, ImGuiWindowFlags_NoNav);

		// Setup ImGuizmo
		ImGuizmo::SetOrthographic(false);
		ImGuizmo::SetDrawlist();

		ImGuizmo::SetRect(m_ViewportBounds.x, m_ViewportBounds.y, m_SceneSize.x, m_SceneSize.y);

		m_HoverViewport = ImGui::IsWindowHovered();

		const auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
		const auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
		const auto viewportOffset = ImGui::GetWindowPos();
		m_ViewportBounds = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y,
			viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };
		const uint32_t textureID = 0; // m_Framebuffer->GetColorAttachmentRendererID();

		ImVec2 scenePanelSize = ImGui::GetContentRegionAvail();
		if (m_SceneSize != glm::vec2(scenePanelSize.x, scenePanelSize.y))
		{
			// m_Framebuffer->Resize(static_cast<int>(scenePanelSize.x), static_cast<int>(scenePanelSize.y));
			m_SceneSize = {scenePanelSize.x, scenePanelSize.y};
		}
		auto io = ImGui::GetIO();
		static float zoom = 1.0f;

		ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();

		ImGui::Image(reinterpret_cast<ImTextureID>(static_cast<size_t>(textureID)),
			     ImVec2{m_SceneSize.x, m_SceneSize.y},
			     ImVec2{0, 1}, ImVec2{1, 0});

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FILE_PATH"))
			{
				const auto* path = static_cast<const char*>(payload->Data);
				OpenScene(AssetsManager::GetAssetAbsolutePath(path));
			}
			ImGui::EndDragDropTarget();
		}

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2);
		ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1, 1, 1, 1));
		const auto& colors = ImGui::GetStyle().Colors;
		const auto& buttonHovered = colors[ImGuiCol_ButtonHovered];
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f));
		const auto& buttonActive = colors[ImGuiCol_ButtonActive];
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(buttonActive.x, buttonActive.y, buttonActive.z, 0.5f));

		Texture2D* icon = m_EditorState == EditorState::Edit ? m_IconPlay.get() : m_IconStop.get();

		const auto playButtonSize = 40;
		ImGui::SetCursorScreenPos(ImVec2(canvas_p0.x + (m_SceneSize.x - playButtonSize) / 2.0f, canvas_p0.y + 10));
		if (ImGui::ImageButton(reinterpret_cast<ImTextureID>(static_cast<size_t>(0)), ImVec2(playButtonSize, playButtonSize), ImVec2(0, 0), ImVec2(1, 1)))
		{
			switch (m_EditorState)
			{
				case EditorState::Edit:
					m_EditorState = EditorState::Play;
					m_LastEditorScene = Scene::Copy(*m_CurrentScene);
					m_CurrentScene->OnRuntimeStart();
					break;
				case EditorState::Play:
					m_CurrentScene->OnRuntimeStop();
					m_EditorState = EditorState::Edit;
					ReloadScene();
					break;
			}
		}

		ImGui::PopStyleVar(3);
		ImGui::PopStyleColor(4);

		if (ImGui::IsItemHovered())
		{
			zoom += io.MouseWheel / 10.0f;
			zoom = std::max(zoom, 0.25f);
		}
//		ImVec2 canvas_p1 = ImVec2(canvas_p0.x + m_SceneSize.x, canvas_p0.y + m_SceneSize.y);
//
//		auto drawLists = ImGui::GetWindowDrawList();
//		drawLists->PushClipRect(canvas_p0, canvas_p1, true);
//
//		const float GRID_STEP = 64.0f * zoom;
//		for (float x = fmodf(0, GRID_STEP); x < m_SceneSize.x; x += GRID_STEP)
//			drawLists->AddLine(ImVec2(canvas_p0.x + x, canvas_p0.y), ImVec2(canvas_p0.x + x, canvas_p1.y), IM_COL32(200, 200, 200, 40));
//		for (float y = fmodf(0, GRID_STEP); y < m_SceneSize.y; y += GRID_STEP)
//			drawLists->AddLine(ImVec2(canvas_p0.x, canvas_p0.y + y), ImVec2(canvas_p1.x, canvas_p0.y + y), IM_COL32(200, 200, 200, 40));
//
//		drawLists->PopClipRect();

		m_EditorCamera.SetViewportSize(m_SceneSize.x, m_SceneSize.y);
		m_CurrentScene->OnViewportResize(m_SceneSize.x, m_SceneSize.y);

		if (m_SelectedEntity && m_EditorState == EditorState::Edit)
		{

			// Editor camera
			const glm::mat4& cameraProjection = m_EditorCamera.GetCamera().getProjectionMatrix();
			glm::mat4 cameraView = m_EditorCamera.GetCamera().getViewMatrix();
			auto& transformComponent = m_SelectedEntity.getComponent<Component::Transform>();
			glm::mat4 transform = transformComponent.getTransform();

			ImGuizmo::Manipulate(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection),
					     ImGuizmo::TRANSLATE, ImGuizmo::LOCAL, glm::value_ptr(transform),
					     nullptr, nullptr);

			static glm::vec3 position;
			static bool isUsing = false;
			if (ImGuizmo::IsUsing())
			{
				if (!isUsing) {
					position = transformComponent.position;
				}
				isUsing = true;
				glm::vec3 translation, scale, skew;
				glm::quat rotation;
				glm::vec4 perspective;
				glm::decompose(transform, scale, rotation, translation, skew, perspective);
				transformComponent.rotation = glm::eulerAngles(rotation);
				transformComponent.position = translation;
				transformComponent.scale = scale;
			} else {
				if (isUsing) {
					isUsing = false;
					Log::info("{}, {}, {}", position.x, position.y, position.z);
					auto& uuid = m_SelectedEntity.getComponent<Component::ID>();
					AddCommand(std::make_unique<UpdateTransformPositionCommand>(uuid.uuid, position));
					position = transformComponent.position;
				}
			}
		}
		ImGui::End();
		ImGui::PopStyleVar();
	}

	void CardiaTor::OnEvent(Event& event)
	{
		if (m_HoverViewport)
		{
			// Mouse inside viewport
			m_EditorCamera.OnEvent(event);
		}

		EventDispatcher dispatcher(event);
		// shortcuts
		dispatcher.dispatch<KeyDownEvent>([this](const KeyDownEvent& e)
		{
			auto ctrl = Input::isKeyPressed(Key::LeftCtrl) || Input::isKeyPressed(Key::RightCtrl);
			auto shift = Input::isKeyPressed(Key::LeftShift) || Input::isKeyPressed(Key::LeftShift);
			if (ctrl)
			{
				switch (e.getKeyCode())
				{
				case Key::O: OpenProject();
					break;
				case Key::S:
					Log::info("Saving...");
					SaveScene();
					break;
				case Key::W:
				{
					if (shift) {
						RedoCommand();
						break;
					}
					UndoCommand();
					break;
				}
				default:
					break;
				}
			}
		});

		dispatcher.dispatch<MouseButtonDownEvent>([this](const MouseButtonDownEvent& e)
		{
			if (e.getButton() == Mouse::Left)
			{
				if (m_SelectedEntity && ImGuizmo::IsOver()) return;
				if (!m_HoverViewport) return;

				m_SelectedEntity = m_HoveredEntity;
				auto inspector = m_PanelManager.GetLastFocused<Panel::InspectorPanel>();
				if (inspector)
					inspector->SetSelectedEntity(m_HoveredEntity);

				auto sceneHierarchy = m_PanelManager.GetLastFocused<Panel::SceneHierarchyPanel>();
				if (sceneHierarchy)
					sceneHierarchy->SetSelectedEntity(m_HoveredEntity);
			}
		});
	}

	void CardiaTor::AddCommand(std::unique_ptr<Command> command) {
		m_UnusedCommand.emplace(std::move(command));
		if (!m_UnusedCommand.empty()) m_UsedCommand = std::stack<std::unique_ptr<Command>>();
	}

	void CardiaTor::UndoCommand()
	{
		if (m_UnusedCommand.empty()) return;
		auto command = std::move(m_UnusedCommand.top());
		Log::info("Undo...");
		command->Undo(this);

		m_UnusedCommand.pop();
		m_UsedCommand.push(std::move(command));
	}

	void CardiaTor::RedoCommand()
	{
		if (m_UsedCommand.empty()) return;
		auto command = std::move(m_UsedCommand.top());
		Log::info("Redo...");
		command->Redo(this);

		m_UsedCommand.pop();
		m_UnusedCommand.push(std::move(command));
	}

	void CardiaTor::SetSelectedEntity(Entity entity)
	{
		m_SelectedEntity = entity;
	}
}
