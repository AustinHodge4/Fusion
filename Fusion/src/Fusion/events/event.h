#pragma once

#include "fusion/core.h"

namespace fusion {

	// Events in Fusion are currently blocking, meaning when an event occurs it
	// immediately gets dispatched and must be dealt with right then an there.
	// For the future, a better strategy might be to buffer events in an event
	// bus and process them during the "event" part of the update stage.

	enum class EventType
	{
		None = 0,
		WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
		AppTick, AppUpdate, AppRender,
		KeyPressed, KeyReleased, KeyTyped,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
	};

	enum EventCategory
	{
		None = 0,
		EventCategoryApplication = BIT(0),
		EventCategoryInput = BIT(1),
		EventCategoryKeyboard = BIT(2),
		EventCategoryMouse = BIT(3),
		EventCategoryMouseButton = BIT(4)
	};

#define EVENT_CLASS_TYPE(type) static EventType get_static_type() { return EventType::type; }\
								virtual EventType get_event_type() const override { return get_static_type(); }\
								virtual const char* get_name() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) virtual int get_category_flags() const override { return category; }

	class FUSION_API Event
	{
	public:
		virtual ~Event() = default;

		bool handled = false;

		virtual EventType get_event_type() const = 0;
		virtual const char* get_name() const = 0;
		virtual int get_category_flags() const = 0;
		virtual std::string to_string() const { return get_name(); }

		bool is_in_category(EventCategory category)
		{
			return get_category_flags() & category;
		}
	};

	class EventDispatcher
	{
	public:
		EventDispatcher(Event& event)
			: _event(event)
		{
		}

		// F will be deduced by the compiler
		template<typename T, typename F>
		bool Dispatch(const F& func)
		{
			if (_event.get_event_type() == T::get_static_type())
			{
				_event.handled |= func(static_cast<T&>(_event));
				return true;
			}
			return false;
		}
	private:
		Event& _event;
	};

	inline std::ostream& operator<<(std::ostream& os, const Event& e)
	{
		return os << e.to_string();
	}
}