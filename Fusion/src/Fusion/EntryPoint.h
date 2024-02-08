#pragma once

#ifdef FE_PLATFORM_WINDOWS

extern fusion::Application* fusion::create_application();

int main(int argc, char** argv) 
{
	fusion::Log::init();
	FE_CORE_TRACE("Init Log!");
	FE_INFO("Hello, Var={0}", 2);

	FE_CORE_INFO("Initializing Input Keys...");
	fusion::Key::initialize();

	auto app = fusion::create_application();
	app->run();
	delete app;
}

#endif