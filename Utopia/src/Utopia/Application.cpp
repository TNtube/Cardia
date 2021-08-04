#include "utpch.hpp"
#include "Application.hpp"
#include "Utopia/Event.hpp"
#include "Utopia/Log.hpp"


namespace Utopia
{
	Application::Application()
	{
		
	}
	
	Application::~Application()
	{
		
	}

	void Application::Run()
	{
		KeyDownEvent e(98, 0);
		UT_TRACE(e);
		while (true);
	}
}
