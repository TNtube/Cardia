#include <Cardia.hpp>
#include <Cardia/EntryPoint.hpp>

#include "EditorLayer.hpp"

class CardiaTor : public Cardia::Application
{
public:
	CardiaTor()
	{
		pushLayer(layer.get());
	}

	~CardiaTor() override = default;

private:
	std::unique_ptr<Cardia::Layer> layer = std::make_unique<Cardia::EditorLayer>("SandBox2D");
};


std::unique_ptr<Cardia::Application> Cardia::CreateApplication()
{
	return std::make_unique<CardiaTor>();
}
