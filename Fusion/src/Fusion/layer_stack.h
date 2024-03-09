#pragma once

#include "core.h"
#include "layer.h"

namespace fusion {

	class FUSION_API LayerStack
	{
	public:
		LayerStack();
		~LayerStack();

		void push_layer(Layer* layer);
		void push_overlay(Layer* layer);
		void pop_layer(Layer* layer);
		void pop_overlay(Layer* layer);

		std::vector<Layer*>::iterator begin() { return _layers.begin(); }
		std::vector<Layer*>::iterator end() { return _layers.end(); }
	private:
		std::vector<Layer*> _layers;
		unsigned int _layer_insert_index = 0;
	};

}

