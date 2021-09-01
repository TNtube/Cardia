#pragma once

#ifdef CD_PLATFORM_WINDOWS

extern std::unique_ptr<Cardia::Application> Cardia::CreateApplication();

int main(int argc, char** argv)
{
	Cardia::Logger::Init();

	const auto app = Cardia::CreateApplication();
	app->Run();
}

#endif
