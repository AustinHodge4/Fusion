#pragma once

#include "fusion/core.h"

#include "fusion/input/input_keys.h"
#include "platform/platform_input_manager.h"

namespace fusion {

	class KeyEvent;

	class FUSION_API Input 
	{
	public:
		friend PlatformInput;
		friend Key;
		friend KeyEvent;

		inline static bool is_key_pressed(VirtualKey keycode) { return PlatformInput::is_key_pressed_impl(keycode); }
		inline static bool is_mouse_button_pressed(VirtualKey button) { return PlatformInput::is_mouse_button_pressed_impl(button); }
		inline static float get_mouse_x() { return PlatformInput::get_mouse_x_impl(); }
		inline static float get_mouse_y() { return PlatformInput::get_mouse_y_impl(); }
		inline static std::pair<float, float> get_mouse_pos() { return PlatformInput::get_mouse_pos_impl(); }
	protected:
		static VirtualKey get_key_from_codes(const int keycode);
		static int get_codes_from_key(const VirtualKey key);

		static void initialize_keys();

	private:
		static std::unordered_map<int, VirtualKey> _key_map;
	};
}