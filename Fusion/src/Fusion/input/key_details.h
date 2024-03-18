#pragma once

#include "fusion/core.h"
#include "virtual_key.h"

namespace fusion {

	class FUSION_API KeyDetails
	{
	public:
		enum KeyFlags
		{
			GamepadKey = 1 << 0,
			MouseButton = 1 << 1,
			ModifierKey = 1 << 2,
			Axis1D = 1 << 3,
			Axis2D = 1 << 4,
			ButtonAxis = 1 << 5,

			NoFlags = 0,
		};

		KeyDetails(const VirtualKey& input_key, const std::string& display_name, const uint32_t key_flags = 0);

	public:
		VirtualKey& get_key() { return _input_key; }
		const std::string& get_display_name() const { return _display_name; }

		inline const bool is_gamepad_key() const { return _is_gamepad; }
		inline const bool is_modifier_key() const { return _is_modifier; }
		inline const bool is_mouse_button() const { return _is_mouse_button; }
	private:
		void initialize(const uint32_t key_flags = 0);

		VirtualKey _input_key;
		std::string _display_name;
		
		bool _is_gamepad;
		bool _is_modifier;
		bool _is_mouse_button;
	};
}