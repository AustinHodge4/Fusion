#pragma once

#include "core.h"

#include "window.h"
#include "fusion/layer_stack.h"

#include "fusion/events/event.h"
#include "fusion/events/application_event.h"

namespace fusion {

	class FUSION_API Application
	{
	public:
		Application();
		virtual ~Application();

		void run();

		void on_event(Event& e);

		void push_layer(Layer* layer);
		void push_overlay(Layer* layer);

		inline static Application& get() { return *_instance; }
		inline Window& get_window() { return *_window; }

	private:
		bool on_window_closed(WindowCloseEvent& e);
	private:
		std::unique_ptr<Window> _window;
		bool _running = true;
		LayerStack _layer_stack;

		inline static Application* _instance;
	};

	Application* create_application();

}

