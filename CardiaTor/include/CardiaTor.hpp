#pragma once


#include <Cardia.hpp>
#include <random>
#include <stack>
#include <imgui.h>

#include "EditorCamera.hpp"
#include "Cardia/Renderer/RenderPass.hpp"
#include "Panels/PanelManager.hpp"
#include "Command/Commands.hpp"
#include "Cardia/Assets/AssetsManager.hpp"


namespace Cardia
{
	enum class EditorState
	{
		Play,
		Edit
	};

	struct OffscreenFrameData
	{
		OffscreenFrameData(const Renderer& renderer, RenderPass renderPass, Texture colorTexture, Texture depthTexture, Framebuffer framebuffer);
		RenderPass CurrentRenderPass;
		Texture ColorTexture;
		std::unique_ptr<DescriptorSet> ColorTextureDescriptorSet;
		Texture DepthTexture;
		std::unique_ptr<DescriptorSet> DepthTextureDescriptorSet;
		Framebuffer CurrentFrameBuffer;
	private:
		const Renderer& m_Renderer;
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
		void OnImGuiDraw();
		void SetSelectedEntity(Entity entity);

		EditorCamera& GetEditorCamera() { return m_EditorCamera; }
		AssetsManager& GetAssetsManager() { return m_AssetsManager; }

		Scene* GetCurrentScene() override { return m_CurrentScene.get(); }

	private:
		void CreateOffscreenFrameData();
		void EnableDocking();
		void OpenProject();
		void InvalidateProject();
		void InvalidateScene();
		void SaveScene() const;
		void OpenScene(const UUID& id);
		void ReloadScene();
		void UndoCommand();
		void RedoCommand();

		std::unique_ptr<ImGuiLayer> m_ImGuiLayer;

		std::unique_ptr<OffscreenFrameData> m_OffscreenFrameData;

		std::shared_ptr<Texture> m_IconPlay;
		ImTextureID m_IconPlayDescriptorSet {};
		std::shared_ptr<Texture> m_IconStop;
		ImTextureID m_IconStopDescriptorSet {};

		std::unique_ptr<Scene> m_CurrentScene;
		std::unique_ptr<Scene> m_LastEditorScene;

		PanelManager m_PanelManager;

		std::stack<std::unique_ptr<Command>> m_UnusedCommand;
		std::stack<std::unique_ptr<Command>> m_UsedCommand;

		Entity m_HoveredEntity;
		Entity m_SelectedEntity;

		Vector2f m_SceneSize {};

		EditorCamera m_EditorCamera {Radianf::FromDegree(45.f), 0.1f, 10000.0f};
		Vector4f m_ViewportBounds{};

		bool m_HoverViewport = false;

		EditorState m_EditorState = EditorState::Edit;
		bool m_EditorStateUpdated = false;

		std::random_device rd;
		std::mt19937 random{rd()};
	};
}
