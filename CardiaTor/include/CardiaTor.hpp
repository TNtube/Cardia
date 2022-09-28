#pragma once


#include <Cardia.hpp>
#include <random>

#include "EditorCamera.hpp"
#include "Panels/SceneHierarchy.hpp"
#include "Panels/DebugPanel.hpp"
#include "Panels/FileHierarchy.hpp"
#include "Panels/InspectorPanel.hpp"


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
		void onUpdate() override;
		void onEvent(Event& event) override;
		void onImGuiDraw() override;

	private:
		void enableDocking();
		void openWorkspace();
		void invalidateWorkspace() const;
		void saveScene() const;
		void openScene(const std::filesystem::path& scenePath);
		void reloadScene();
		std::unique_ptr<Texture2D> m_IconPlay;
		std::unique_ptr<Texture2D> m_IconStop;
		std::unique_ptr<Framebuffer> m_Framebuffer;

		std::unique_ptr<Scene> m_CurrentScene;
		Entity m_SelectedEntity {};
		std::unordered_map<std::string, std::unique_ptr<IPanel>> m_Panels;

		glm::vec2 m_SceneSize {};

		EditorCamera m_EditorCamera;
		glm::vec4 m_ViewportBounds{};

		float m_AspectRatio = static_cast<float>(getWindow().getWidth())
				      / static_cast<float>(getWindow().getHeight());

		bool m_HoverViewport = false;

		EditorState m_EditorState = EditorState::Edit;
		std::string m_LastEditorState {};

		std::random_device rd;
		std::mt19937 random{rd()};
	};
}
