#pragma once


#include "Cardia/Core/Core.hpp"
#include "Cardia/Core/Time.hpp"
#include "Cardia/Core/Window.hpp"
#include "Cardia/ImGui/ImGuiLayer.hpp"


namespace Cardia
{
	class Application
	{
	public:
		Application();
		virtual ~Application() = default;

		void Run();
		virtual void onEvent(Event& e) = 0;
		virtual void onUpdate(DeltaTime deltaTime) = 0;
		virtual void onImGuiDraw(DeltaTime deltaTime) = 0;
		bool onWinClose(WindowCloseEvent& e);

		inline static Application& get() { return *s_Instance; }
		inline Window& getWindow() const { return *m_Window; }
		inline void close() { m_Running = false; }

	private:
		std::unique_ptr<Window> m_Window;
		std::unique_ptr<ImGuiLayer> m_ImGuiLayer;
		DeltaTime m_DeltaTime;
		bool m_Running = true;
		static Application* s_Instance;

	};

	std::unique_ptr<Application> CreateApplication();
}
