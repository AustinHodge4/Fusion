#include "fepch.h"
#include "application.h"

#include <GLFW/glfw3.h>

namespace fusion {

	Application::Application()
	{
		_window = std::unique_ptr<Window>(Window::create());
		_window->set_event_callback(FE_BIND_EVENT_FN(Application::on_event));
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

	void Application::on_event(Event& e)
	{
		FE_CORE_INFO("{0}", e);

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(FE_BIND_EVENT_FN(on_window_closed));
	}

	bool Application::on_window_closed(WindowCloseEvent& e)
	{
		_running = false;
		return true;
	}


}