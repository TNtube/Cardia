#pragma once


#include "Cardia/Core/Core.hpp"
#include "Cardia/Core/Time.hpp"
#include "Cardia/Core/Window.hpp"
#include "Cardia/ImGui/ImGuiLayer.hpp"
#include "Cardia/Scripting/ScriptEngine.hpp"


namespace Cardia
{
	struct ProjectSettings
	{
	public:
		std::string workspace;

	};
	class Application
	{
	public:
		Application();
		virtual ~Application() = default;

		void Run();
		virtual void onEvent(Event& e) = 0;
		virtual void onUpdate() = 0;
		virtual void onImGuiDraw() = 0;
		bool onWinClose(WindowCloseEvent& e);

		inline static ProjectSettings& projectSettings() { return s_ProjectSettings; };
		inline static Application& get() { return *s_Instance; }
		inline Window& getWindow() const { return *m_Window; }
		inline void close() { m_Running = false; }

	private:
		std::unique_ptr<Window> m_Window;
		std::unique_ptr<ImGuiLayer> m_ImGuiLayer;
		ScriptEngine m_ScriptEngine;
		bool m_Running = true;
		inline static ProjectSettings s_ProjectSettings {};
		static Application* s_Instance;

	};

	std::unique_ptr<Application> CreateApplication();
}
