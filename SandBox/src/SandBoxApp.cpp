#include "SandBox2D.hpp"

#include <Cardia/EntryPoint.hpp>


std::unique_ptr<Cardia::Application> Cardia::CreateApplication()
{
	return std::make_unique<SandBox2D>();
}
