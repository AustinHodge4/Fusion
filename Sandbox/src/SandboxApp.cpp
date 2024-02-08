#include <fusion.h>

class ExampleLayer : public fusion::Layer
{
public:
	ExampleLayer() : Layer("Example") {}

	void on_update() override
	{
		if (fusion::Input::is_key_pressed(fusion::Key::W))
		{
			FE_TRACE("Pressed W key (poll)");
		}
		
	}

	void on_event(fusion::Event& e) override
	{
		if (e.get_event_type() == fusion::EventType::KeyPressed) {
			fusion::KeyPressedEvent& event = (fusion::KeyPressedEvent&)e;
			if (event.get_key() == fusion::Key::W) {
				FE_TRACE("Pressed {0} key (event)!", fusion::Key::get_key_details(fusion::Key::W)->get_display_name());
			}
			FE_TRACE("{0}", event.get_key().get_name());
		}
	}
};

class Sandbox : public fusion::Application
{
public:
	Sandbox()
	{
		push_layer(new ExampleLayer());
		push_overlay(new fusion::ImGUILayer());
	}

	~Sandbox()
	{

	}
};

fusion::Application* fusion::create_application() 
{
	return new Sandbox();
}