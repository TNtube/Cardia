#include <Utopia.hpp>


class LayerTest : public Utopia::Layer
{
public:
	LayerTest()
		: Layer("Test") {}

	void onUpdate() override
	{
		UT_INFO("LayerTest is updating...");
	}

	void onEvent(Utopia::Event& event) override
	{
		UT_WARN(event);
	}
};


class SandBox : public Utopia::Application
{
public:
	SandBox()
	{
		pushLayer(layer.get());

		pushOverlay(imGuiLayer.get());
	}

	~SandBox()
	{

	}

	std::unique_ptr<Utopia::Layer> layer = std::make_unique<LayerTest>();
	std::unique_ptr<Utopia::Layer> imGuiLayer = std::make_unique<Utopia::ImGuiLayer>();
};


std::unique_ptr<Utopia::Application> Utopia::CreateApplication()
{
	return std::make_unique<SandBox>();
} 