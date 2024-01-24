#pragma once

#include "event.h"

namespace fusion {

	class FUSION_API MouseMovedEvent : public Event
	{
	public:
		MouseMovedEvent(const float x, const float y)
			: _mouse_x(x), _mouse_y(y) {}

		inline float get_x() const { return _mouse_x; }
		inline float get_y() const { return _mouse_y; }

		std::string to_string() const override
		{
			std::stringstream ss;
			ss << "MouseMovedEvent: " << _mouse_x << ", " << _mouse_y;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseMoved)
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
	private:
		float _mouse_x, _mouse_y;
	};

	class FUSION_API MouseScrolledEvent : public Event
	{
	public:
		MouseScrolledEvent(const float offset_x, const float offset_y)
			: _offset_x(offset_x), _offset_y(offset_y) {}

		float get_offset_x() const { return _offset_x; }
		float get_offset_y() const { return _offset_y; }

		std::string to_string() const override
		{
			std::stringstream ss;
			ss << "MouseScrolledEvent: " << get_offset_x() << ", " << get_offset_y();
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseScrolled)
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
	private:
		float _offset_x, _offset_y;
	};

	class FUSION_API MouseButtonEvent : public Event
	{
	public:
		inline int get_mouse_button() const { return _button; }

		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput | EventCategoryMouseButton)
	protected:
		MouseButtonEvent(int button)
			: _button(button) {}

		int _button;
	};

	class FUSION_API MouseButtonPressedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonPressedEvent(int button)
			: MouseButtonEvent(button) {}

		std::string to_string() const override
		{
			std::stringstream ss;
			ss << "MouseButtonPressedEvent: " << _button;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseButtonPressed)
	};

	class FUSION_API MouseButtonReleasedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonReleasedEvent(int button)
			: MouseButtonEvent(button) {}

		std::string to_string() const override
		{
			std::stringstream ss;
			ss << "MouseButtonReleasedEvent: " << _button;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseButtonReleased)
	};

}