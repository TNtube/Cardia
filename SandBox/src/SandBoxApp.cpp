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
		pushLayer(std::make_shared<LayerTest>());
	}

	~SandBox()
	{
		
	}
};


Utopia::Application* Utopia::CreateApplication()
{
	return new SandBox;
} 