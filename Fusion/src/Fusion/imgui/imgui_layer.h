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

		void on_attach() override;
		void on_detach() override;

		void on_update() override;
		void on_event(Event& e) override;

	private:
		bool on_mouse_scrolled_event(MouseScrolledEvent& e);
		bool on_mouse_button_press_event(MouseButtonPressedEvent& e);
		bool on_mouse_button_released_event(MouseButtonReleasedEvent& e);
		bool on_mouse_moved_event(MouseMovedEvent& e);

		bool on_key_pressed_event(KeyPressedEvent& e);
		bool on_key_released_event(KeyReleasedEvent& e);
		bool on_key_typed_event(KeyTypedEvent& e);

		bool on_window_resized_event(WindowResizeEvent& e);
	private:
		float _time = 0.0f;
	};
}