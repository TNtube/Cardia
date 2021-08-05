#pragma once

#include "Utopia/Core.hpp"
#include "Utopia/Window.hpp"


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

	private:
		std::unique_ptr<Window> m_Window;
		bool m_Running = true;
	};

	Application* CreateApplication();
}


