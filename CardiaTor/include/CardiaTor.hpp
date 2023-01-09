#pragma once


#include <Cardia.hpp>
#include <random>
#include <stack>

#include "EditorCamera.hpp"
#include "Panels/PanelManager.hpp"
#include "Command/Commands.hpp"


namespace Cardia
{
	enum class EditorState
	{
		Play,
		Edit
	};
	class CardiaTor : public Application
	{
	public:
		CardiaTor();

		~CardiaTor() override = default;
		void AddCommand(std::unique_ptr<Command> command);
		void OnUpdate() override;
		void OnEvent(Event& event) override;
		void OnImGuiDraw() override;

		Scene* GetCurrentScene() override { return m_CurrentScene.get(); }

	private:
		void EnableDocking();
		void OpenWorkspace();
		void InvalidateWorkspace();
		void SaveScene() const;
		void OpenScene(const std::filesystem::path& scenePath);
		void ReloadScene();
		void UndoCommand();
		void RedoCommand();
		std::unique_ptr<Texture2D> m_IconPlay;
		std::unique_ptr<Texture2D> m_IconStop;
		std::unique_ptr<Framebuffer> m_Framebuffer;

		std::unique_ptr<Scene> m_CurrentScene;

		PanelManager m_PanelManager;

		std::stack<std::unique_ptr<Command>> m_UnusedCommand;
		std::stack<std::unique_ptr<Command>> m_UsedCommand;

		Entity m_HoveredEntity;

		glm::vec2 m_SceneSize {};

		EditorCamera m_EditorCamera;
		glm::vec4 m_ViewportBounds{};

		bool m_HoverViewport = false;

		EditorState m_EditorState = EditorState::Edit;
		std::string m_LastEditorState {};

		std::random_device rd;
		std::mt19937 random{rd()};
	};
}
