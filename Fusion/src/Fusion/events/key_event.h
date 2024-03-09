#pragma once

#include "event.h"

#include "fusion/input/input.h"
#include "fusion/input/virtual_key.h"

namespace fusion {

	class FUSION_API KeyEvent : public Event
	{
	public:
		inline int get_keycode() const { return _keycode; }
		inline VirtualKey get_key() const { return Input::get_key_from_codes(_keycode); }

		EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)
	protected:
		KeyEvent(int keycode)
			: _keycode(keycode) {}

		int _keycode;
	};

	class FUSION_API KeyPressedEvent : public KeyEvent
	{
	public:
		KeyPressedEvent(int keycode, int repeats)
			: KeyEvent(keycode), _repeats(repeats) {}

		inline int get_repeat_count() const { return _repeats; }

		std::string to_string() const override
		{
			std::stringstream ss;
			ss << "KeyPressedEvent: " << _keycode << " (repeats = " << _repeats << ")";
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyPressed)
	private:
		int _repeats;
	};

	class FUSION_API KeyReleasedEvent : public KeyEvent
	{
	public:
		KeyReleasedEvent(int keycode)
			: KeyEvent(keycode) {}

		std::string to_string() const override
		{
			std::stringstream ss;
			ss << "KeyReleasedEvent: " << _keycode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyReleased)
	};

	class FUSION_API KeyTypedEvent : public KeyEvent
	{
	public:
		KeyTypedEvent(int keycode)
			: KeyEvent(keycode) {}

		std::string to_string() const override
		{
			std::stringstream ss;
			ss << "KeyTypedEvent: " << _keycode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyTyped)
	};
}