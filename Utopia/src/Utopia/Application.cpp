#include "utpch.h"
#include "Application.h"
#include "Utopia/Event.h"
#include "Utopia/Log.h"


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
