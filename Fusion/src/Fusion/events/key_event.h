#pragma once

#include "event.h"

#include "fusion/input/input.h"
#include "fusion/input/virtual_key.h"

namespace Fusion {

	class FUSION_API KeyEvent : public Event
	{
	public:
		inline int GetKeycode() const { return _keycode; }
		inline const VirtualKey& GetKey() const { return Input::GetKeyFromCodes(_keycode); }

		EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)
	protected:
		KeyEvent(int p_keycode)
			: _keycode(p_keycode) {}

		int _keycode;
	};

	class FUSION_API KeyPressedEvent : public KeyEvent
	{
	public:
		KeyPressedEvent(int p_keycode, int p_repeats)
			: KeyEvent(p_keycode), _repeats(p_repeats) {}

		inline int GetRepeatCount() const { return _repeats; }

		std::string ToString() const override
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
		KeyReleasedEvent(int p_keycode)
			: KeyEvent(p_keycode) {}

		std::string ToString() const override
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
		KeyTypedEvent(int p_keycode)
			: KeyEvent(p_keycode) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyTypedEvent: " << _keycode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyTyped)
	};
}