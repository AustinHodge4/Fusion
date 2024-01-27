#include <fusion.h>

class ExampleLayer : public fusion::Layer
{
public:
	ExampleLayer() : Layer("Example") {}

	void on_update() override
	{
		FE_INFO("ExampleLayer::Update");
	}

	void on_event(fusion::Event& e) override
	{
		FE_TRACE("{0}", e);
	}
};

class sandbox : public fusion::Application
{
public:
	sandbox()
	{
		push_layer(new ExampleLayer());
	}

	~sandbox()
	{

	}
};

fusion::Application* fusion::create_application() 
{
	return new sandbox();
}