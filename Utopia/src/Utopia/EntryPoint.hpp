#pragma once

#ifdef UT_PLATFORM_WINDOWS

extern std::unique_ptr<Utopia::Application> Utopia::CreateApplication();

int main(int argc, char** argv)
{
	Utopia::Logger::Init();
	
	auto app = Utopia::CreateApplication();
	app->Run();
}

#endif