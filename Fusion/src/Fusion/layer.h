#pragma once

#include "core.h"
#include "events/event.h"

namespace fusion {

	class FUSION_API Layer
	{
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer();

		virtual void on_attach() {}
		virtual void on_detach() {}
		virtual void on_update() {}
		virtual void on_event(Event& e) {}

		inline const std::string& get_name() const { return _name; }
	protected:
		std::string _name;
	};

}

