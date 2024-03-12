#pragma once

#include "core.h"

#include "window.h"

#include "events/event.h"
#include "events/application_event.h"

#include "layer_stack.h"
#include "imgui/imgui_layer.h"

#include "renderer/shader.h"
#include "renderer/buffer.h"

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
		inline static Application* _instance;

		std::unique_ptr<Window> _window;
		ImGUILayer* _imgui_layer;

		bool _running = true;
		LayerStack _layer_stack;

		unsigned int _vertexArray;
		std::unique_ptr<Shader> _shader;
		std::unique_ptr<VertexBuffer> _vertexBuffer;
		std::unique_ptr<IndexBuffer> _indexBuffer;
	};

	Application* create_application();

}

