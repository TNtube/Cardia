#pragma once


#include "Cardia/Core/Core.hpp"
#include "Cardia/Core/Time.hpp"
#include "Cardia/Core/Window.hpp"
#include "Cardia/ImGui/ImGuiLayer.hpp"
#include "Cardia/Scripting/ScriptEngine.hpp"
#include "Renderer/RenderContext.hpp"


namespace Cardia
{
	class Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();
		virtual void OnEvent(Event& e) = 0;
		virtual void OnUpdate() = 0;
		virtual void OnImGuiDraw() = 0;
		virtual Scene* GetCurrentScene() = 0;
		bool onWinClose(WindowCloseEvent& e);

		inline static Application& get() { return *s_Instance; }
		inline Window& GetWindow() const { return *m_Window; }
		inline void close() { m_Running = false; }

	private:
		std::unique_ptr<Window> m_Window = Window::Create();

	protected:
		RenderContext m_RenderContext {*m_Window};
	private:
		std::unique_ptr<ImGuiLayer> m_ImGuiLayer;
		ScriptEngine m_ScriptEngine;
		bool m_Running = true;
		static Application* s_Instance;
	};

	std::unique_ptr<Application> CreateApplication();
}
