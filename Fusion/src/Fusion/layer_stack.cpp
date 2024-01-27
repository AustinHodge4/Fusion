#include "fepch.h"
#include "layer_stack.h"

namespace fusion {

	LayerStack::LayerStack()
	{
		_layer_insert = _layers.begin();
	}

	LayerStack::~LayerStack()
	{
		for (Layer* layer : _layers)
			delete layer;
	}

	void LayerStack::push_layer(Layer* layer)
	{
		_layer_insert = _layers.emplace(_layer_insert, layer);
		_layer_insert++;
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
			_layers.erase(it);
			_layer_insert--;
		}
	}

	void LayerStack::pop_overlay(Layer* layer)
	{
		auto it = std::find(_layers.begin(), _layers.end(), layer);
		if (it != _layers.end()) 
		{
			_layers.erase(it);
		}
	}
}
