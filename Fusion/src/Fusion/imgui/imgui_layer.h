#pragma once

#include "fusion/core.h"
#include "fusion/layer.h"

#include "fusion/events/application_event.h"
#include "fusion/events/key_event.h"
#include "fusion/events/mouse_event.h"

namespace Fusion {

	class FUSION_API ImGUILayer : public Layer
	{
	public:
		ImGUILayer();
		~ImGUILayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnImguiRender() override;

		void Begin();
		void End();

	private:
		float _time = 0.0f;
	};
}