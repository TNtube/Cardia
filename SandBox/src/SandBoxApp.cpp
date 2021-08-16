#include <Utopia.hpp>


class LayerTest : public Utopia::Layer
{
public:
	explicit LayerTest(std::string&& name)
		: Layer(name) {}

	void onUpdate() override
	{
		Utopia::Log::info(getName());
	}

	void onEvent(Utopia::Event& event) override
	{
		Utopia::Log::warn(event.toString()); // TODO: Remove toString
		if(event.getEventType() == Utopia::EventType::KeyDown){
		}
	}
};


class SandBox : public Utopia::Application
{
public:
	SandBox()
	{
	    pushLayer(layer.get());
	    pushLayer(layer2.get());
	    pushOverlay(imGuiLayer.get());
	}

	~SandBox() override = default;

private:
    std::unique_ptr<Utopia::Layer> layer = std::make_unique<LayerTest>("First Layer");
    std::unique_ptr<Utopia::Layer> layer2 = std::make_unique<LayerTest>("Second Layer");
	std::unique_ptr<Utopia::Layer> imGuiLayer = std::make_unique<Utopia::ImGuiLayer>();
};


std::unique_ptr<Utopia::Application> Utopia::CreateApplication()
{
	return std::make_unique<SandBox>();
} 