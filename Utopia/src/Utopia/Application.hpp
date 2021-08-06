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
		void pushLayer(LayerPtr layer);
		void pushOverlay(LayerPtr overlay);

	private:
		std::unique_ptr<Window> m_Window;
		bool m_Running = true;
		LayerStack m_LayerStack;
	};

	Application* CreateApplication();
}
