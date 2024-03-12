#pragma once

#include "fusion/input/virtual_key.h"

namespace fusion {
	class FUSION_API WindowsInput
	{
	public:
		static bool is_key_pressed_impl(const VirtualKey& keycode);
		static bool is_mouse_button_pressed_impl(const VirtualKey& button);
		static std::pair<float, float> get_mouse_pos_impl();
		static float get_mouse_x_impl();
		static float get_mouse_y_impl();

		static unsigned int get_platform_keys(unsigned int* keycodes, std::string* key_names);
	};
	typedef WindowsInput PlatformInput;
}