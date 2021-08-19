#include <Utopia.hpp>
#include <imgui.h>


class LayerTest : public Utopia::Layer
{
public:
	explicit LayerTest(std::string&& name)
		: Layer(name) {}

	void onUpdate() override
	{
		if(Utopia::Input::isKeyPressed(Utopia::Key::Y))
		{
			Utopia::Log::info("Key Y is pressed");
		}
	}

	void onEvent(Utopia::Event& event) override
	{
		// Utopia::Log::trace(event);
	}

	void onImGuiDraw() override
	{
		std::string foo(getName());
		ImGui::Begin(("Hello " + foo).c_str());
		ImGui::Text("Coucou toi !");
		ImGui::Button("Foo");
		ImGui::End();
	}
};


class SandBox : public Utopia::Application
{
public:
	SandBox()
	{
		pushLayer(layer.get());
		pushLayer(layer2.get());
	}

	~SandBox() override = default;

private:
	std::unique_ptr<Utopia::Layer> layer = std::make_unique<LayerTest>("First Layer");
	std::unique_ptr<Utopia::Layer> layer2 = std::make_unique<LayerTest>("Second Layer");
};


std::unique_ptr<Utopia::Application> Utopia::CreateApplication()
{
	return std::make_unique<SandBox>();
}
