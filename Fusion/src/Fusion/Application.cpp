#include "fepch.h"
#include "application.h"

#include "input/input.h"

namespace Fusion {

	Application::Application()
	{
		FE_CORE_ASSERT(!_instance, "Application already exists!");
		_instance = this;

		_window = std::unique_ptr<Window>(Window::Create());
		_window->SetEventCallback(FE_BIND_EVENT_FN(Application::OnEvent));

		_imguiLayer = new ImGUILayer();
		PushOverlay(_imguiLayer);
	}

	Application::~Application()
	{
		
	}

	void Application::PushLayer(Layer* p_layer)
	{
		_layerStack.PushLayer(p_layer);
		p_layer->OnAttach();
	}

	void Application::PushOverlay(Layer* p_layer)
	{
		_layerStack.PushOverlay(p_layer);
		p_layer->OnAttach();
	}

	void Application::Run()
	{		
		while (_running)
		{
			for (Layer* layer : _layerStack)
			{
				layer->OnUpdate();
			}

			_imguiLayer->Begin();
			for (Layer* layer : _layerStack)
			{
				layer->OnImguiRender();
			}
			_imguiLayer->End();

			_window->OnUpdate();
		}
	}

	void Application::OnEvent(Event& p_event)
	{
		//FE_CORE_INFO("{0}", p_event);

		EventDispatcher dispatcher(p_event);
		dispatcher.Dispatch<WindowCloseEvent>(FE_BIND_EVENT_FN(OnWindowClosed));

		for (auto it = _layerStack.end(); it != _layerStack.begin();)
		{
			if (p_event.handled)
				break;
			(*--it)->OnEvent(p_event);
		}
	}

	bool Application::OnWindowClosed(WindowCloseEvent& p_event)
	{
		_running = false;
		return true;
	}


}