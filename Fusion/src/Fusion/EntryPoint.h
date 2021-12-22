#pragma once

#ifdef FE_PLATFORM_WINDOWS

extern Fusion::Application* Fusion::CreateApplication();

int main(int argc, char** argv) 
{
	auto app = Fusion::CreateApplication();
	app->Run();
	delete app;
}

#endif