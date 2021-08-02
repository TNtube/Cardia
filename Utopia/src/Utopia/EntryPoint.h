#pragma once

#ifdef UT_PLATFORM_WINDOWS

extern Utopia::Application* Utopia::CreateApplication();

int main(int argc, char** argv)
{
	std::cout << "Hello Utopia" << std::endl;
	auto app = Utopia::CreateApplication();
	app->Run();
	delete app;
}

#endif