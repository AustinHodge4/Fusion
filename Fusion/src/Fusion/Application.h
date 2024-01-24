#pragma once

#include "core.h"
#include "events/event.h"

#include "window.h"

namespace fusion {

	class FUSION_API Application
	{
	public:
		Application();
		virtual ~Application();

		void run();
	private:
		std::unique_ptr<Window> _window;
		bool _running = true;
	};

	Application* create_application();

}

