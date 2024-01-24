#include "fepch.h"
#include "application.h"

#include "fusion/events/application_event.h"

#include <GLFW/glfw3.h>

namespace fusion {
	Application::Application()
	{
		_window = std::unique_ptr<Window>(Window::create());
	}

	Application::~Application()
	{
	}

	void Application::run()
	{		
		while (_running)
		{
			glClearColor(1, 0, 0, 1);
			glClear(GL_COLOR_BUFFER_BIT);
			_window->on_update();
		}
	}


}