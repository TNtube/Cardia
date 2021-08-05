#pragma once

#ifdef UT_PLATFORM_WINDOWS

extern Utopia::Application* Utopia::CreateApplication();

int main(int argc, char** argv)
{
	Utopia::Log::Init();
	
	auto app = Utopia::CreateApplication();
	app->Run();
	delete app;
}

#endif