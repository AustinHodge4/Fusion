#pragma once

#include "layer.h"

#include <vector>

namespace Fusion {

	class FUSION_API LayerStack
	{
	public:
		LayerStack();
		~LayerStack();

		void PushLayer(Layer* p_layer);
		void PushOverlay(Layer* p_layer);
		void PopLayer(Layer* p_layer);
		void PopOverlay(Layer* p_layer);

		std::vector<Layer*>::iterator begin() { return _layers.begin(); }
		std::vector<Layer*>::iterator end() { return _layers.end(); }
	private:
		std::vector<Layer*> _layers;
		unsigned int _insertIndex = 0;
	};

}

