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

		std::shared_ptr<Utopia::Layer> layer = std::make_unique<LayerTest>();
		pushLayer(layer);

		std::shared_ptr<Utopia::Layer> imGuiLayer = std::make_unique<Utopia::ImGuiLayer>();
		pushOverlay(imGuiLayer);
	}

	~SandBox()
	{

	}
};


std::unique_ptr<Utopia::Application> Utopia::CreateApplication()
{
	return std::make_unique<SandBox>();
} 