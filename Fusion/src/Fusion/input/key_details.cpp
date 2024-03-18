#include "fepch.h"
#include "key_details.h"

namespace fusion {

	KeyDetails::KeyDetails(const VirtualKey& input_key, const std::string& display_name, const uint32_t key_flags)
		: _input_key(input_key), _display_name(display_name)
	{
		initialize(key_flags);
	}

	void KeyDetails::initialize(const uint32_t key_flags)
	{
		_is_gamepad = (key_flags & KeyDetails::GamepadKey) != 0;
		_is_modifier = (key_flags & KeyDetails::ModifierKey) != 0;
		_is_mouse_button = (key_flags & KeyDetails::MouseButton) != 0;
	}
}
