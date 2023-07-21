#pragma once


#include <Cardia.hpp>
#include <random>
#include <stack>

#include "EditorCamera.hpp"
#include "Cardia/Renderer/RenderPass.hpp"
#include "Panels/PanelManager.hpp"
#include "Command/Commands.hpp"


namespace Cardia
{
	enum class EditorState
	{
		Play,
		Edit
	};

	struct OffscreenFrameData
	{
		OffscreenFrameData(RenderPass renderPass, Texture2D colorTexture, Texture2D depthTexture, Framebuffer framebuffer)
			: RenderPass{std::move(renderPass)},
			  ColorTexture{std::move(colorTexture)},
			  DepthTexture{std::move(depthTexture)},
			  Framebuffer{std::move(framebuffer)}
		{}
		RenderPass RenderPass;
		Texture2D ColorTexture;
		Texture2D DepthTexture;
		Framebuffer Framebuffer;
	};


	class CardiaTor : public Application
	{
	public:
		CardiaTor(const CardiaTor& other) = delete;
		CardiaTor& operator=(const CardiaTor& other) = delete;
		CardiaTor(CardiaTor&& other) noexcept = delete;
		CardiaTor& operator=(CardiaTor&& other) noexcept = delete;

		CardiaTor();

		~CardiaTor() override = default;
		void AddCommand(std::unique_ptr<Command> command);
		void OnUpdate() override;
		void OnRender() override;
		void OnEvent(Event& event) override;
		void OnImGuiDraw() override;
		void SetSelectedEntity(Entity entity);

		EditorCamera& GetEditorCamera() { return m_EditorCamera; }
		Renderer& GetRenderer() { return m_Renderer; }

		Scene* GetCurrentScene() override { return m_CurrentScene.get(); }

	private:
		void CreateOffscreenFrameData();
		void EnableDocking();
		void OpenProject();
		void InvalidateProject();
		void InvalidateScene();
		void SaveScene() const;
		void OpenScene(const std::filesystem::path& scenePath);
		void ReloadScene();
		void UndoCommand();
		void RedoCommand();

		std::unique_ptr<ImGuiLayer> m_ImGuiLayer;

		std::unique_ptr<OffscreenFrameData> m_OffscreenFrameData;

		std::shared_ptr<Texture2D> m_IconPlay;
		std::shared_ptr<Texture2D> m_IconStop;

		std::unique_ptr<Scene> m_CurrentScene;
		std::unique_ptr<Scene> m_LastEditorScene;

		PanelManager m_PanelManager;

		std::stack<std::unique_ptr<Command>> m_UnusedCommand;
		std::stack<std::unique_ptr<Command>> m_UsedCommand;

		Entity m_HoveredEntity;
		Entity m_SelectedEntity;

		glm::vec2 m_SceneSize {};

		EditorCamera m_EditorCamera {45.f, 0.1f, 10000.0f};
		glm::vec4 m_ViewportBounds{};

		bool m_HoverViewport = false;

		EditorState m_EditorState = EditorState::Edit;

		std::random_device rd;
		std::mt19937 random{rd()};
	};
}
