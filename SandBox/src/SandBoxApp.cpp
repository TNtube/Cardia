#include "Cardia.hpp"
#include <imgui.h>


class LayerTest : public Cardia::Layer
{
public:
	explicit LayerTest(std::string&& name)
		: Layer(name) {}

	void onUpdate() override
	{
		if (Cardia::Input::isKeyPressed(Cardia::Key::Y))
		{
			Cardia::Log::info("Key Y is pressed");
		}
	}

	void onEvent(Cardia::Event& event) override
	{
		// Cardia::Log::trace(event);
	}

	void onImGuiDraw() override
	{
		/*
		std::string foo(getName());
		ImGui::Begin(("Hello " + foo).c_str());
		ImGui::Text("Coucou toi !");
		ImGui::Button("Foo");
		ImGui::End();
		*/
	}
};


class SandBox : public Cardia::Application
{
public:
	SandBox()
	{
		pushLayer(layer.get());
		pushLayer(layer2.get());
	}

	~SandBox() override = default;

private:
	std::unique_ptr<Cardia::Layer> layer = std::make_unique<LayerTest>("First Layer");
	std::unique_ptr<Cardia::Layer> layer2 = std::make_unique<LayerTest>("Second Layer");
};


std::unique_ptr<Cardia::Application> Cardia::CreateApplication()
{
	return std::make_unique<SandBox>();
}
