#pragma once

#include "fusion/events/event.h"
#include "timestep.h"

namespace Fusion {

	class FUSION_API Layer
	{
	public:
		Layer(const std::string& p_name = "Layer");
		virtual ~Layer();

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(Timestep p_timestep) {}
		virtual void OnImguiRender() {}
		virtual void OnEvent(Event& p_event) {}

		inline const std::string& GetName() const { return _name; }
	protected:
		std::string _name;
	};

}

