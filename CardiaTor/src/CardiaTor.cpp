#include "CardiaTor.hpp"

#include <Cardia.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <imgui.h>
#include <ImGuizmo.h>
#include <nfd.h>
#include <ranges>
#include <Cardia/SerializationUtils/SceneSerializer.hpp>


namespace Cardia
{
	CardiaTor::CardiaTor()
	{
		const auto &window = getWindow();

		m_CurrentScene = std::make_unique<Scene>("Default Scene");

		m_Panels = std::unordered_map<std::string, std::unique_ptr<IPanel>>();

		m_Panels.insert({"SceneHierarchy", std::make_unique<Panel::SceneHierarchy>(m_CurrentScene.get())});
		m_Panels.insert({"Inspector", std::make_unique<Panel::InspectorPanel>()});
		m_Panels.insert({"Debug", std::make_unique<Panel::DebugPanel>()});
		m_Panels.insert({"FileHierarchy", std::make_unique<Panel::FileHierarchy>()});

		m_IconPlay = Texture2D::create("assets/icons/play.png");
		m_IconStop = Texture2D::create("assets/icons/pause.png");

		const FramebufferSpec spec{ window.getWidth(), window.getHeight() };
		m_Framebuffer = Framebuffer::create(spec);
	}


	void CardiaTor::onUpdate(DeltaTime deltaTime)
	{
		m_Framebuffer->bind();
		RenderAPI::get().setClearColor({0.2f, 0.2f, 0.2f, 1});
		RenderAPI::get().clear();


		if (m_EditorState == EditorState::Edit)
		{
			m_CurrentScene->onUpdateEditor(deltaTime, m_EditorCamera);
			m_EditorCamera.onUpdate(deltaTime);
		}
		if (m_EditorState == EditorState::Play)
		{
			m_CurrentScene->onUpdateRuntime(deltaTime);
		}

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
					if (ImGui::MenuItem("Open", "CTRL+O"))
					{
						openWorkspace();
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

	void CardiaTor::openWorkspace()
	{
		char *outPath = nullptr;
		const nfdresult_t result = NFD_PickFolder( nullptr, &outPath );
        
		if ( result == NFD_OKAY ) {
			auto& settings = projectSettings();
			settings.workspace = std::string(outPath);
			invalidateWorkspace();
			Log::coreInfo("Workspace : {0}", settings.workspace);
		}
	}

	void CardiaTor::invalidateWorkspace() const
	{
		for (const auto& [name, panel]: m_Panels)
		{
			panel->updateWorkspace(projectSettings().workspace);
		}
	}

	void CardiaTor::saveScene() const
	{
		if (m_CurrentScene->path.empty())
		{
			nfdchar_t *outPath = nullptr;
			NFD_SaveDialog("cardia", "", &outPath);
			if (!outPath)
			{
				return;
			}
			m_CurrentScene->path = outPath;
			if (m_CurrentScene->path.extension() != ".cardia")
			{
				m_CurrentScene->path += ".cardia";
			}
		}
		
		std::ofstream file;
		file.open(m_CurrentScene->path);
		file << SerializerUtils::SerializeScene(m_CurrentScene.get(), projectSettings().workspace);
		file.close();
	}

	void CardiaTor::openScene(const std::filesystem::path& scenePath)
	{
		const std::ifstream t(scenePath);
		std::stringstream buffer;
		buffer << t.rdbuf();

		auto newScene = std::make_unique<Scene>(scenePath.filename().string());
		newScene->path = scenePath;
		
		if (!SerializerUtils::DeserializeScene(buffer.str(), *newScene, projectSettings().workspace))
		{
			Log::coreInfo("Unable to load {0}", scenePath.string());
		} else
		{
			m_CurrentScene = std::move(newScene);
		}
		dynamic_cast<Panel::SceneHierarchy*>(m_Panels.at("SceneHierarchy").get())->setCurrentScene(m_CurrentScene.get());
		m_SelectedEntity = {};
	}

	void CardiaTor::reloadScene()
	{
		const auto scenePath = m_CurrentScene->path;
		m_CurrentScene = std::make_unique<Scene>(scenePath.filename().string());
		m_CurrentScene->path = scenePath;

		if (!SerializerUtils::DeserializeScene(m_LastEditorState, *m_CurrentScene, projectSettings().workspace))
		{
			Log::coreInfo("Unable to reload {0}", m_CurrentScene->path.filename().string());
		}
		dynamic_cast<Panel::SceneHierarchy*>(m_Panels.at("SceneHierarchy").get())->setCurrentScene(m_CurrentScene.get());
		m_SelectedEntity = {};
	}

	void CardiaTor::onImGuiDraw(DeltaTime deltaTime)
	{
		enableDocking();

		m_SelectedEntity = m_Panels.at("SceneHierarchy")->getSelectedEntity();

		for (const auto& [name, panel] : m_Panels)
		{
			panel->updateSelectedEntity(m_SelectedEntity);
			panel->onImGuiRender(deltaTime);
		}

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
		const auto& colors = ImGui::GetStyle().Colors;
		const auto& buttonHovered = colors[ImGuiCol_ButtonHovered];
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f));
		const auto& buttonActive = colors[ImGuiCol_ButtonActive];
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(buttonActive.x, buttonActive.y, buttonActive.z, 0.5f));

		ImGui::Begin("##toolbar", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

		float size = ImGui::GetWindowHeight() - 4.0f;
		Texture2D* icon = m_EditorState == EditorState::Edit ? m_IconPlay.get() : m_IconStop.get();
		ImGui::SetCursorPosX((ImGui::GetWindowContentRegionMax().x- size) * 0.5f);
		if (ImGui::ImageButton(reinterpret_cast<ImTextureID>(static_cast<size_t>(icon->getRendererID())), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0))
		{
			if (m_EditorState == EditorState::Edit)
			{
				m_EditorState = EditorState::Play;
				m_LastEditorState = SerializerUtils::SerializeScene(m_CurrentScene.get(), projectSettings().workspace);
			}
			else if (m_EditorState == EditorState::Play)
			{
				m_EditorState = EditorState::Edit;
				reloadScene();
			}
		}
		ImGui::PopStyleVar(2);
		ImGui::PopStyleColor(3);
		ImGui::End();

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

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("FILE_PATH"))
			{
				const auto* path = static_cast<const char*>(payload->Data);
				openScene(path);
			}
			ImGui::EndDragDropTarget();
		}

		m_AspectRatio = m_SceneSize.x / m_SceneSize.y;

		m_EditorCamera.setViewportSize(m_SceneSize.x, m_SceneSize.y);
		m_CurrentScene->onViewportResize(m_SceneSize.x, m_SceneSize.y);

		if (m_SelectedEntity && m_EditorState == EditorState::Edit)
		{
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();

			ImGuizmo::SetRect(m_ViewportBounds.x, m_ViewportBounds.y, m_SceneSize.x, m_SceneSize.y);

			// Editor camera
			const glm::mat4& cameraProjection = m_EditorCamera.getProjectionMatrix();
			glm::mat4 cameraView = m_EditorCamera.getViewMatrix();
			auto& transformComponent = m_SelectedEntity.getComponent<Component::Transform>();
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

		EventDispatcher dispatcher(event);
		// shortcuts
		dispatcher.dispatch<KeyDownEvent>([this](const KeyDownEvent& e)
		{
			if (Input::isKeyPressed(Key::LeftCtrl) || Input::isKeyPressed(Key::RightCtrl))
			{
				switch (e.getKeyCode())
				{
				case Key::O:
					openWorkspace();
					break;
				case Key::S:
					saveScene();
					break;
				default:
					break;
				}
			}
		});

		dispatcher.dispatch<WindowFocusEvent>([this](const WindowFocusEvent& e)
		{
			if (e.isFocused())
			{
				const auto view = m_CurrentScene->getRegistry().view<Component::Script>();
				for (auto [entity, behavior] : view.each())
				{
					behavior.setPath(behavior.getPath());
				}
			}
		});
	}
}
