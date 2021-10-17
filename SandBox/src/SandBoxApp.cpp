#include <Cardia.hpp>
#include <Cardia/EntryPoint.hpp>

#include "SandBox2D.hpp"

class SandBox : public Cardia::Application
{
public:
	SandBox()
	{
		pushLayer(layer.get());
	}

	~SandBox() override = default;

private:
	std::unique_ptr<Cardia::Layer> layer = std::make_unique<SandBox2D>("SandBox2D");
};


std::unique_ptr<Cardia::Application> Cardia::CreateApplication()
{
	return std::make_unique<SandBox>();
}
