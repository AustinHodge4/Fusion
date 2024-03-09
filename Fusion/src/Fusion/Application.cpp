#include "fepch.h"
#include "application.h"

#include <glad/glad.h>
#include "input/input.h"

namespace fusion {

	Application::Application()
	{
		FE_CORE_ASSERT(!_instance, "Application already exists!");
		_instance = this;

		_window = std::unique_ptr<Window>(Window::create());
		_window->set_event_callback(FE_BIND_EVENT_FN(Application::on_event));

		_imgui_layer = new ImGUILayer();
		push_overlay(_imgui_layer);

		glGenVertexArrays(1, &_vertexArray);
		glBindVertexArray(_vertexArray);

		glGenBuffers(1, &_vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, _vertexBuffer);

		float vertices[3 * 3] = {
			-0.5f, -0.5f, 0.0f,
			0.5f, -0.5f, 0.0f,
			0.0f, 0.5f, 0.0f,
		};

		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, nullptr);

		glGenBuffers(1, &_indexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBuffer);

		unsigned int indices[3] = {
			0, 1, 2
		};

		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
		
		glBindVertexArray(0);
	}

	Application::~Application()
	{
		glDeleteBuffers(1, &_vertexBuffer);
		glDeleteBuffers(1, &_indexBuffer);
		glDeleteVertexArrays(1, &_vertexArray);
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
			glClearColor(0.1f, 0.1f, 0.1f, 1);
			glClear(GL_COLOR_BUFFER_BIT);

			glBindVertexArray(_vertexArray);
			glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr);

			for (Layer* layer : _layer_stack)
			{
				layer->on_update();
			}

			_imgui_layer->begin();
			for (Layer* layer : _layer_stack)
			{
				layer->on_imgui_render();
			}
			_imgui_layer->end();

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
			if (e.handled)
				break;
			(*--it)->on_event(e);
		}
	}

	bool Application::on_window_closed(WindowCloseEvent& e)
	{
		_running = false;
		return true;
	}


}