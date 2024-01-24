#include <fusion.h>

class sandbox : public fusion::Application
{
public:
	sandbox()
	{
	
	}

	~sandbox()
	{

	}
};

fusion::Application* fusion::create_application() 
{
	return new sandbox();
}