#pragma once

#if defined(_WIN64) || defined(__linux__)

extern std::unique_ptr<Cardia::Application> Cardia::CreateApplication();

int main(int argc, char** argv)
{
	Cardia::Logger::Init();

	const auto app = Cardia::CreateApplication();
	app->Run();
}

#endif
