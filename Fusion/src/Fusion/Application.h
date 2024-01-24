#pragma once

#include "core.h"
#include "events/event.h"
#include "fusion/events/application_event.h"

#include "window.h"

namespace fusion {

	class FUSION_API Application
	{
	public:
		Application();
		virtual ~Application();

		void run();

		void on_event(Event& e);

	private:
		bool on_window_closed(WindowCloseEvent& e);
	private:
		std::unique_ptr<Window> _window;
		bool _running = true;
	};

	Application* create_application();

}

