#pragma once

#include "fepch.h"

#include "fusion/core.h"
#include "fusion/events/event.h"

namespace fusion {

	struct WindowProps
	{
		std::string title;
		unsigned int width;
		unsigned int height;

		WindowProps(const std::string& title = "Fusion Engine",
					unsigned int width = 1280,
					unsigned int height = 720)
			: title(title), width(width), height(height)
		{}
	};

	class FUSION_API Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window() {}

		virtual void on_update() = 0;

		virtual unsigned int get_width() const = 0;
		virtual unsigned int get_height() const = 0;

		virtual void set_event_callback(const EventCallbackFn& callback) = 0;
		virtual void set_vsync(bool enabled) = 0;
		virtual bool is_vsync() const = 0;

		static Window* create(const WindowProps& props = WindowProps());
	};
}