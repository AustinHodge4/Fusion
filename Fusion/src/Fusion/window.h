#pragma once

#include "fepch.h"

#include "core.h"
#include "events/event.h"

namespace Fusion {

	struct WindowProps
	{
		std::string title;
		unsigned int width;
		unsigned int height;

		WindowProps(const std::string& p_title = "Fusion Engine",
					unsigned int p_width = 1280,
					unsigned int p_height = 720)
			: title(p_title), width(p_width), height(p_height)
		{}
	};

	class FUSION_API Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window() {}

		virtual void OnUpdate() = 0;

		virtual unsigned int GetWidth() const = 0;
		virtual unsigned int GetHeight() const = 0;

		virtual void SetEventCallback(const EventCallbackFn& p_callback) = 0;
		virtual void SetVsync(bool p_enabled) = 0;
		virtual bool IsVsync() const = 0;

		virtual void* GetNativeWindow() const = 0;

		static Window* Create(const WindowProps& p_props = WindowProps());
	};
}