#pragma once

#include "core.h"
#include "events/event.h"

namespace Fusion {

	class FUSION_API Layer
	{
	public:
		Layer(const std::string& p_name = "Layer");
		virtual ~Layer();

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate() {}
		virtual void OnImguiRender() {}
		virtual void OnEvent(Event& p_event) {}

		inline const std::string& GetName() const { return _name; }
	protected:
		std::string _name;
	};

}

