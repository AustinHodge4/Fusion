#include <Fusion.h>

class Sandbox : public Fusion::Application
{
public:
	Sandbox()
	{
	
	}

	~Sandbox()
	{

	}
};

Fusion::Application* Fusion::CreateApplication() 
{
	return new Sandbox();
}