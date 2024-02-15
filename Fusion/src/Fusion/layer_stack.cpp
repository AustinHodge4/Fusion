#include "fepch.h"
#include "layer_stack.h"

namespace fusion {

	LayerStack::LayerStack()
	{
		
	}

	LayerStack::~LayerStack()
	{
		for (Layer* layer : _layers)
		{
			layer->on_detach();
			delete layer;
		}
	}

	void LayerStack::push_layer(Layer* layer)
	{
		_layers.emplace(_layers.begin() + _layer_insert_index, layer);
		_layer_insert_index++;
	}

	void LayerStack::push_overlay(Layer* layer)
	{
		_layers.emplace_back(layer);
	}

	void LayerStack::pop_layer(Layer* layer)
	{
		auto it = std::find(_layers.begin(), _layers.end(), layer);
		if (it != _layers.end())
		{
			layer->on_detach();
			_layers.erase(it);
			_layer_insert_index--;
		}
	}

	void LayerStack::pop_overlay(Layer* layer)
	{
		auto it = std::find(_layers.begin(), _layers.end(), layer);
		if (it != _layers.end()) 
		{
			layer->on_detach();
			_layers.erase(it);
		}
	}
}
