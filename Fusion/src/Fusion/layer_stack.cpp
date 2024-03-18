#include "fepch.h"
#include "layer_stack.h"

namespace Fusion {

	LayerStack::LayerStack()
	{
		
	}

	LayerStack::~LayerStack()
	{
		for (Layer* layer : _layers)
		{
			layer->OnDetach();
			delete layer;
		}
	}

	void LayerStack::PushLayer(Layer* p_layer)
	{
		_layers.emplace(_layers.begin() + _insertIndex, p_layer);
		_insertIndex++;
	}

	void LayerStack::PushOverlay(Layer* p_layer)
	{
		_layers.emplace_back(p_layer);
	}

	void LayerStack::PopLayer(Layer* p_layer)
	{
		auto it = std::find(_layers.begin(), _layers.end(), p_layer);
		if (it != _layers.end())
		{
			p_layer->OnDetach();
			_layers.erase(it);
			_insertIndex--;
		}
	}

	void LayerStack::PopOverlay(Layer* p_layer)
	{
		auto it = std::find(_layers.begin(), _layers.end(), p_layer);
		if (it != _layers.end()) 
		{
			p_layer->OnDetach();
			_layers.erase(it);
		}
	}
}
