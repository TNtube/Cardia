#pragma once


#include "Cardia/Core/Core.hpp"
#include "Cardia/Core/Time.hpp"
#include "Cardia/Core/Window.hpp"
#include "Cardia/ImGui/ImGuiLayer.hpp"
#include "Cardia/Scripting/ScriptEngine.hpp"
#include "Renderer/Renderer.hpp"
#include "Cardia/Asset/AssetsManager.hpp"


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
		virtual void OnRender() = 0;
		virtual Scene* GetCurrentScene() = 0;
		bool OnWinClose(WindowCloseEvent& e);

		inline static Application& Get() { return *s_Instance; }
		inline Window& GetWindow() const { return *m_Window; }
		inline void Close() { m_Running = false; }
		inline Renderer& GetRenderer() { return m_Renderer; }
		inline AssetsManager& GetAssetsManager() { return m_AssetsManager; }

	private:
		std::unique_ptr<Window> m_Window = Window::Create();

	protected:
		Renderer m_Renderer {*m_Window};
		AssetsManager m_AssetsManager {m_Renderer};
	private:
		ScriptEngine m_ScriptEngine;
		bool m_Running = true;
		static Application* s_Instance;
	};

	std::unique_ptr<Application> CreateApplication();
}
