#pragma once

#include "core.h"
#include "application.h"

#if defined(FE_PLATFORM_WINDOWS) || defined(FE_PLATFORM_LINUX)

extern Fusion::Application* Fusion::CreateApp();

int main(int argc, char** argv) 
{
	Fusion::Log::Initialize();
	FE_CORE_TRACE("Init Log!");
	FE_INFO("Hello, Var={0}", 2);

	// TODO: Gamepad support & cross platform
	FE_CORE_INFO("Initializing Input Keys...");
	Fusion::Key::Initialize();

	auto app = Fusion::CreateApp();
	app->Run();
	delete app;
}

#endif