#pragma once

#include "window.h"

#include "fusion/events/event.h"
#include "fusion/events/application_event.h"

#include "layer_stack.h"
#include "fusion/imgui/imgui_layer.h"

#include "timestep.h"

namespace Fusion {

	class FUSION_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();

		void OnEvent(Event& p_event);

		void PushLayer(Layer* p_layer);
		void PushOverlay(Layer* p_layer);

		inline static Application& Get() { return *_instance; }
		inline Window& GetWindow() { return *_window; }

	private:
		bool OnWindowClosed(WindowCloseEvent& p_event);
	private:
		inline static Application* _instance;

		std::unique_ptr<Window> _window;
		ImGUILayer* _imguiLayer;

		bool _running = true;
		LayerStack _layerStack;

		float _lastFrameTime = 0.0f;
	};

	Application* CreateApp();

}

