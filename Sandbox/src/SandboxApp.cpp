#include <fusion.h>

class sandbox : public fusion::application
{
public:
	sandbox()
	{
	
	}

	~sandbox()
	{

	}
};

fusion::application* fusion::create_application() 
{
	return new sandbox();
}