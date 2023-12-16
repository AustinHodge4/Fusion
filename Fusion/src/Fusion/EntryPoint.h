#pragma once

#ifdef FE_PLATFORM_WINDOWS

extern fusion::application* fusion::create_application();

int main(int argc, char** argv) 
{
	fusion::log::init();
	FE_CORE_TRACE("Init Log!");
	FE_INFO("Hello, Var={0}", 2);

	auto app = fusion::create_application();
	app->run();
	delete app;
}

#endif