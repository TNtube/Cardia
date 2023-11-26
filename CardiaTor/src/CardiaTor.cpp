#include "CardiaTor.hpp"

#include <imgui.h>
#include <ImGuizmo.h>
#include <nfd.h>

#include <Cardia.hpp>
#include <Cardia/Project/Project.hpp>
#include <Cardia/Assets/AssetsManager.hpp>

#include "Cardia/Serialization/Serializer.hpp"
#include "Panels/SceneHierarchyPanel.hpp"
#include "Panels/DebugPanel.hpp"
#include "Panels/FileHierarchyPanel.hpp"
#include "Panels/InspectorPanel.hpp"
#include "Panels/ConsolePanel.hpp"
#include "Cardia/ImGui/imgui_impl_vulkan.h"


namespace Cardia
{
	CardiaTor::CardiaTor()
	{
		CreateOffscreenFrameData();

		m_ImGuiLayer = std::make_unique<ImGuiLayer>(m_Renderer);

		m_PanelManager.CreatePanel<Panel::SceneHierarchyPanel>();
		m_PanelManager.CreatePanel<Panel::InspectorPanel>();
		m_PanelManager.CreatePanel<Panel::DebugPanel>();
		m_PanelManager.CreatePanel<Panel::FileHierarchyPanel>();
		m_PanelManager.CreatePanel<Panel::ConsolePanel>();

		m_CurrentScene = std::make_unique<Scene>(m_Renderer);

		for (const auto& panel: m_PanelManager.Panels()) {
			panel->OnSceneLoad(m_CurrentScene.get());
		}


		const auto playHandle = m_AssetsManager.GetHandleFromRelative("resources/icons/play.png");
		m_IconPlay = m_AssetsManager.Load<Texture>(playHandle);
		if (m_IconPlay)
			m_IconPlayDescriptorSet = ImGui_ImplVulkan_AddTexture(m_IconPlay->GetSampler(), m_IconPlay->GetView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		const auto pauseHandle = m_AssetsManager.GetHandleFromRelative("resources/icons/pause.png");
		m_IconStop = m_AssetsManager.Load<Texture>(pauseHandle);
		if (m_IconStop)
			m_IconStopDescriptorSet = ImGui_ImplVulkan_AddTexture(m_IconStop->GetSampler(), m_IconStop->GetView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		ImGuiIO &io = ImGui::GetIO();
		io.IniFilename = "resources/editorconfig.ini";

		Logger::AddEditorCallback([&](const spdlog::level::level_enum& level, const std::string& msg) {
			m_PanelManager.GetLastFocused<Panel::ConsolePanel>()->AddLog(level, msg);
		});
	}


	void CardiaTor::OnUpdate()
	{
		const auto view = m_CurrentScene->GetRegistry().view<Cardia::Component::Transform>();
		for (auto entity : view) {
			auto& transform = view.get<Cardia::Component::Transform>(entity);

			if (transform.IsDirty())
			{
				transform.RecomputeWorld({entity, m_CurrentScene.get()});
			}
		}

		if (m_EditorStateUpdated)
		{
			switch (m_EditorState)
			{
				case EditorState::Play:
					m_LastEditorScene = Scene::Copy(*m_CurrentScene);
					m_CurrentScene->OnRuntimeStart();
					break;
				case EditorState::Edit:
					m_CurrentScene->OnRuntimeStop();
					ReloadScene();
					break;
			}
			m_EditorStateUpdated = false;
		}


		if (m_EditorState == EditorState::Edit)
		{
			m_EditorCamera.OnUpdate();
		} else if (m_EditorState == EditorState::Play)
		{
			ScriptEngine::Instance().OnRuntimeUpdate();
		}

		auto[mouseX, mouseY] = ImGui::GetMousePos();
		mouseX -= m_ViewportBounds.x;
		mouseY -= m_ViewportBounds.y;

		const Vector2f viewportSize = Vector2f(m_ViewportBounds.z - m_ViewportBounds.x, m_ViewportBounds.w - m_ViewportBounds.y);
		mouseY = viewportSize.y - mouseY;

		if (m_CurrentScene)
		{
			if (mouseX >= 0 && mouseY >= 0 && mouseX < viewportSize.x && mouseY < viewportSize.y)
			{
				// int pixelData = m_Framebuffer->ReadPixel(1, mouseX, mouseY);
				// m_HoveredEntity = pixelData == -1 ? Entity() : Entity((entt::entity)pixelData, m_CurrentScene.get());
			}
		}
	}

	void CardiaTor::OnRender()
	{
		bool failed;
		if (const auto commandBuffer = m_Renderer.Begin())
		{
			// ImGui processing
			m_ImGuiLayer->Begin();
			OnImGuiDraw();
			m_ImGuiLayer->End();

			// Offscreen rendering
			m_Renderer.BeginRenderPass(m_OffscreenFrameData->CurrentFrameBuffer, m_OffscreenFrameData->CurrentRenderPass);

			// Render Scene
			if (m_EditorState == EditorState::Edit)
			{
				m_CurrentScene->OnRender(commandBuffer, m_EditorCamera.GetCamera(), m_EditorCamera.GetTransformComponent());
			} else if (m_EditorState == EditorState::Play)
			{
				m_CurrentScene->OnRuntimeRender(commandBuffer);
			}

			m_Renderer.EndRenderPass();

			// Draw to swapchain
			m_Renderer.BeginSwapChainRenderPass();
			m_ImGuiLayer->Render(commandBuffer);
			m_Renderer.EndRenderPass();
			failed = m_Renderer.End();
		} else
		{
			failed = true;
		}

		// If the swapchain is out of date or the pass failed, recreate offscreen framebuffer
		if (failed)
		{
			CreateOffscreenFrameData();
		}
	}

	void CardiaTor::CreateOffscreenFrameData()
	{
		const auto& swapChain = m_Renderer.GetSwapChain();
		const auto extent = swapChain.GetExtent();
		auto imageFormat = swapChain.GetSwapChainImageFormat();
		auto depthFormat = swapChain.FindDepthFormat();

		VkAttachmentDescription colorAttachment = {};
		colorAttachment.format = imageFormat;
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		VkAttachmentReference colorAttachmentRef;
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentDescription depthAttachment{};
		depthAttachment.format = depthFormat;
		depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depthAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		VkAttachmentReference depthAttachmentRef;
		depthAttachmentRef.attachment = 1;
		depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;
		subpass.pDepthStencilAttachment = &depthAttachmentRef;

		VkSubpassDependency dependency = {};

		dependency.dstSubpass = 0;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.srcAccessMask = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

		RenderPassSpecification specification {
			.AttachmentDescriptions = {colorAttachment, depthAttachment},
			.SubpassDescription = subpass,
			.SubpassDependency = dependency
		};

		RenderPass renderPass(m_Renderer.GetDevice(), specification);

		TextureCreateInfo colorCreateInfo {};
		colorCreateInfo.Format = imageFormat;
		colorCreateInfo.UsageFlags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		colorCreateInfo.AspectFlags = VK_IMAGE_ASPECT_COLOR_BIT;

		Texture colorTexture{ m_Renderer.GetDevice(), colorCreateInfo, {extent.width, extent.height} };

		TextureCreateInfo depthCreateInfo {};
		depthCreateInfo.Format = depthFormat;
		depthCreateInfo.UsageFlags = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		depthCreateInfo.AspectFlags = VK_IMAGE_ASPECT_DEPTH_BIT;
		Texture depthTexture{ m_Renderer.GetDevice(), depthCreateInfo, {extent.width, extent.height} };

		const std::vector attachments = {colorTexture.GetView(), depthTexture.GetView()};
		const FramebufferSpecification framebufferSpecification {
			.width = extent.width,
			.height = extent.height,
			.attachments = attachments
		};

		Framebuffer framebuffer{ m_Renderer.GetDevice(), renderPass, framebufferSpecification };

		m_Renderer.GetDevice().WaitIdle();

		m_OffscreenFrameData = std::make_unique<OffscreenFrameData>(
			m_Renderer,
			std::move(renderPass),
			std::move(colorTexture),
			std::move(depthTexture),
			std::move(framebuffer));

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
						Application::Get().Close();
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
			if (const auto project = Project::Load(outPath)) {
				Log::Info("OpeningProject : {0}", project->GetConfig().Name);
				InvalidateProject();
			} else {
				Log::Warn("Failed To LoadImpl Project : {0}", outPath);
			}
		}
	}

	void CardiaTor::InvalidateProject()
	{
		for (const auto& panel : m_PanelManager.Panels())
		{
			panel->OnUpdateWorkspace();
		}
		ScriptEngine::InvalidateProject();
		const auto activeProject = Project::GetActive();
		const auto& config = activeProject->GetConfig();

		m_AssetsManager.PopulateHandleFromProject(*activeProject);

		OpenScene(m_AssetsManager.GetHandleFromAsset(config.StartScene));
	}

	void CardiaTor::InvalidateScene()
	{
		m_HoveredEntity = Entity();
		m_SelectedEntity = Entity();
		for (auto& panel: m_PanelManager.Panels()) {
			panel->OnSceneLoad(m_CurrentScene.get());
		}
		m_CurrentScene->OnViewportResize(m_SceneSize.x, m_SceneSize.y);
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

		Serializer serializer(*m_CurrentScene);
		serializer.Serialize(path);
	}

	void CardiaTor::OpenScene(const AssetHandle& handle)
	{
		const auto scenePath = m_AssetsManager.AbsolutePathFromHandle(handle);

		try {
			auto scene = Serializer<Scene>::Deserialize(scenePath);
			m_CurrentScene = std::make_unique<Scene>(scene);
		} catch (std::exception& e) {
			Log::Error("Failed to load scene: {0}", e.what());
			return;
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
		ImGui::PopStyleVar();

		// Setup ImGuizmo
		ImGuizmo::SetImGuiContext(ImGui::GetCurrentContext());
		ImGuizmo::SetOrthographic(false);
		ImGuizmo::SetDrawlist();
		ImGuizmo::SetRect(m_ViewportBounds.x, m_ViewportBounds.y, m_SceneSize.x, m_SceneSize.y);

		m_HoverViewport = ImGui::IsWindowHovered();

		const auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
		const auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
		const auto viewportOffset = ImGui::GetWindowPos();
		m_ViewportBounds = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y,
			viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

		const auto [width, height] = ImGui::GetContentRegionAvail();
		if (m_SceneSize != Vector2f(width, height))
		{
			m_SceneSize = {width, height};
			// CreateOffscreenFrameData({static_cast<uint32_t>(width), static_cast<uint32_t>(height)});
			m_EditorCamera.SetViewportSize(m_SceneSize.x, m_SceneSize.y);
			m_CurrentScene->OnViewportResize(m_SceneSize.x, m_SceneSize.y);
			ImGui::End();
			return;
		}
		auto io = ImGui::GetIO();
		static float zoom = 1.0f;

		ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();

		ImGui::Image(m_OffscreenFrameData->ColorTextureDescriptorSet->GetDescriptor(),
			     ImVec2{m_SceneSize.x, m_SceneSize.y});

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_HANDLE"))
			{
				const auto* path = static_cast<AssetHandle*>(payload->Data);
				OpenScene(*path);
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

		const auto playButtonSize = 40;
		ImGui::SetCursorScreenPos(ImVec2(canvas_p0.x + (m_SceneSize.x - playButtonSize) / 2.0f, canvas_p0.y + 10));

		ImTextureID set = m_EditorState == EditorState::Edit ? m_IconPlayDescriptorSet : m_IconStopDescriptorSet;
		if (ImGui::ImageButton(set, ImVec2(playButtonSize, playButtonSize), ImVec2(0, 0), ImVec2(1, 1)))
		{
			switch (m_EditorState)
			{
				case EditorState::Edit:
					m_EditorState = EditorState::Play;
					break;
				case EditorState::Play:
					m_EditorState = EditorState::Edit;
					break;
			}

			m_EditorStateUpdated = true;
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

		if (m_SelectedEntity.IsValid() && m_EditorState == EditorState::Edit)
		{

			// Editor camera
			const Matrix4f& cameraProjection = m_EditorCamera.GetCamera().GetProjectionMatrix();
			Matrix4f cameraView = m_EditorCamera.GetTransformMatrix().Inverse();
			const auto& transformComponent = m_SelectedEntity.GetComponent<Component::Transform>();
			Matrix4f transform = transformComponent.GetWorldTransform();

			ImGuizmo::Manipulate(cameraView.Data(), cameraProjection.Data(),
					     ImGuizmo::TRANSLATE, ImGuizmo::LOCAL, transform.Data(),
					     nullptr, nullptr);

			// static Vector3f position;
			// static bool isUsing = false;
			// if (ImGuizmo::IsUsing())
			// {
			// 	if (!isUsing) {
			// 		position = transformComponent.m_Position;
			// 	}
			// 	isUsing = true;
			// 	// TODO: decompose matrix
			// 	// glm::vec3 translation, scale, skew;
			// 	// glm::quat rotation;
			// 	// glm::vec4 perspective;
			// 	// glm::decompose(transform, scale, rotation, translation, skew, perspective);
			// 	// const auto eulerRot = glm::eulerAngles(rotation);
			// 	// transformComponent.position = Vector3f(translation.x, translation.y, translation.z);
			// 	// transformComponent.rotation = Vector3f(eulerRot.x, eulerRot.y, eulerRot.z);
			// 	// transformComponent.scale = Vector3f(scale.x, scale.y, scale.z);
			// } else {
			// 	if (isUsing) {
			// 		isUsing = false;
			// 		Log::Info("{}, {}, {}", position.x, position.y, position.z);
			// 		auto& uuid = m_SelectedEntity.GetComponent<Component::ID>();
			// 		AddCommand(std::make_unique<UpdateTransformPositionCommand>(uuid.Uuid, position));
			// 		position = transformComponent.m_Position;
			// 	}
			// }
		}
		ImGui::End();
	}

	void CardiaTor::OnEvent(Event& event)
	{
		m_ImGuiLayer->OnEvent(event);

		if (m_HoverViewport)
		{
			// Mouse inside viewport
			m_EditorCamera.OnEvent(event);
		}

		EventDispatcher dispatcher(event);
		// shortcuts
		dispatcher.dispatch<KeyDownEvent>([this](const KeyDownEvent& e)
		{
			const auto ctrl = Input::IsKeyPressed(Key::LeftCtrl) || Input::IsKeyPressed(Key::RightCtrl);
			const auto shift = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);
			if (ctrl)
			{
				switch (e.getKeyCode())
				{
				case Key::O:
					Project::Load("C:/dev/Ludum230929/Ludum.cdproj");
					InvalidateProject();
					break;
				case Key::S:
					Log::Info("Saving...");
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
				if (m_SelectedEntity.IsValid() && ImGuizmo::IsOver()) return;
				if (!m_HoverViewport) return;
				if (!m_HoveredEntity.IsValid()) return;

				m_SelectedEntity = m_HoveredEntity;
				const auto inspector = m_PanelManager.GetLastFocused<Panel::InspectorPanel>();
				if (inspector)
					inspector->SetSelectedEntity(m_HoveredEntity);

				const auto sceneHierarchy = m_PanelManager.GetLastFocused<Panel::SceneHierarchyPanel>();
				if (sceneHierarchy)
					sceneHierarchy->SetSelectedEntity(m_HoveredEntity);
			}
		});

		dispatcher.dispatch<WindowResizeEvent>([this](const WindowResizeEvent& e)
		{
			CreateOffscreenFrameData();
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
		Log::Info("Undo...");
		command->Undo(this);

		m_UnusedCommand.pop();
		m_UsedCommand.push(std::move(command));
	}

	void CardiaTor::RedoCommand()
	{
		if (m_UsedCommand.empty()) return;
		auto command = std::move(m_UsedCommand.top());
		Log::Info("Redo...");
		command->Redo(this);

		m_UsedCommand.pop();
		m_UnusedCommand.push(std::move(command));
	}

	void CardiaTor::SetSelectedEntity(Entity entity)
	{
		m_SelectedEntity = entity;
	}

	OffscreenFrameData::OffscreenFrameData(const Renderer &renderer, RenderPass renderPass, Texture colorTexture, Texture depthTexture, Framebuffer framebuffer)
		: m_Renderer(renderer),
		  CurrentRenderPass{std::move(renderPass)},
		  ColorTexture{std::move(colorTexture)},
		  DepthTexture{std::move(depthTexture)},
		  CurrentFrameBuffer{std::move(framebuffer)}
	{
		auto textureLayout = DescriptorSetLayout::Builder(m_Renderer.GetDescriptorLayoutCache())
			.AddBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
			.Build();

		VkDescriptorImageInfo imageBufferInfo;
		imageBufferInfo.sampler = ColorTexture.GetSampler();
		imageBufferInfo.imageView = ColorTexture.GetView();
		imageBufferInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		ColorTextureDescriptorSet =
			DescriptorSet::Writer(m_Renderer.GetDescriptorAllocator(), *textureLayout)
				.WriteImage(0, imageBufferInfo)
				.Build();

		imageBufferInfo.sampler = DepthTexture.GetSampler();
		imageBufferInfo.imageView = DepthTexture.GetView();

		DepthTextureDescriptorSet =
			DescriptorSet::Writer(m_Renderer.GetDescriptorAllocator(), *textureLayout)
				.WriteImage(0, imageBufferInfo)
				.Build();
	}
}
