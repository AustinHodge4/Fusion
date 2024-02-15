#pragma once

#include "fusion/core.h"
#include "fusion/layer.h"

#include "fusion/events/application_event.h"
#include "fusion/events/key_event.h"
#include "fusion/events/mouse_event.h"

namespace fusion {

	class FUSION_API ImGUILayer : public Layer
	{
	public:
		ImGUILayer();
		~ImGUILayer();

		virtual void on_attach() override;
		virtual void on_detach() override;
		virtual void on_imgui_render() override;

		void begin();
		void end();

	private:
		float _time = 0.0f;
	};
}