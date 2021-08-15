#pragma once

#include "Utopia/Core.hpp"
#include "Utopia/Window.hpp"
#include "Utopia/Layer/LayerStack.hpp"


namespace Utopia
{
	class UTOPIA_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();
		void onEvent(Event& e);
		bool onWinClose(WinCloseEvent& e);
		void pushLayer(Layer* layer);
		void pushOverlay(Layer* overlay);

		inline static Application& get() { return *s_Instance; }
		inline Window& getWindow() const { return *m_Window; }

	private:
		std::unique_ptr<Window> m_Window;
		bool m_Running = true;
		LayerStack m_LayerStack;
		static Application* s_Instance;
	};

	std::unique_ptr<Application> CreateApplication();
}
