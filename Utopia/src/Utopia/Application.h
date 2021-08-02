#pragma once

#include "Core.h"


namespace Utopia
{
	class UTOPIA_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();
	};

	Application* CreateApplication();
}


