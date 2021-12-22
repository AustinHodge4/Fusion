#pragma once

#include "Core.h"

namespace Fusion {

	class FUSION_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();
	};

	Application* CreateApplication();

}

