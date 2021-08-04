#pragma once

#ifdef UT_PLATFORM_WINDOWS

extern Utopia::Application* Utopia::CreateApplication();

int main(int argc, char** argv)
{
	Utopia::Log::Init();
	UT_CORE_WARN("Initialized Log !");
	int foo{ 5 };
	UT_INFO("Welcome to Utopia {0}", foo);
	
	auto app = Utopia::CreateApplication();
	app->Run();
	delete app;
}

#endif