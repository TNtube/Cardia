#include "EditorLayer.hpp"

#include <Cardia.hpp>
#include <Cardia/EntryPoint.hpp>

class CardiaTor : public Cardia::Application
{
public:
	CardiaTor()
	{
		pushLayer(m_Layer.get());
	}

	~CardiaTor() override = default;

private:
	std::unique_ptr<Cardia::Layer> m_Layer = std::make_unique<Cardia::EditorLayer>("SandBox2D");
};


std::unique_ptr<Cardia::Application> Cardia::CreateApplication()
{
	return std::make_unique<CardiaTor>();
}
