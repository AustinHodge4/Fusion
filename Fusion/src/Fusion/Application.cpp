#include "fepch.h"
#include "application.h"

#include <glad/glad.h>

namespace fusion {

	Application::Application()
	{
		FE_CORE_ASSERT(!_instance, "Application already exists!");
		_instance = this;

		_window = std::unique_ptr<Window>(Window::create());
		_window->set_event_callback(FE_BIND_EVENT_FN(Application::on_event));

		unsigned int id;
		glGenVertexArrays(1, &id);
	}

	Application::~Application()
	{
		
	}

	void Application::push_layer(Layer* layer)
	{
		_layer_stack.push_layer(layer);
		layer->on_attach();
	}

	void Application::push_overlay(Layer* layer)
	{
		_layer_stack.push_overlay(layer);
		layer->on_attach();
	}

	void Application::run()
	{		
		while (_running)
		{
			glClearColor(1, 0, 0, 1);
			glClear(GL_COLOR_BUFFER_BIT);

			for (Layer* layer : _layer_stack)
			{
				layer->on_update();
			}

			_window->on_update();
		}
	}

	void Application::on_event(Event& e)
	{
		//FE_CORE_INFO("{0}", e);

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(FE_BIND_EVENT_FN(on_window_closed));

		for (auto it = _layer_stack.end(); it != _layer_stack.begin();)
		{
			(*--it)->on_event(e);
			if (e.handled)
				break;
		}
	}

	bool Application::on_window_closed(WindowCloseEvent& e)
	{
		_running = false;
		return true;
	}


}