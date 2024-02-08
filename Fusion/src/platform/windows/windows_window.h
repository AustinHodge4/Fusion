#pragma once

#include "fusion/window.h"

#include <GLFW/glfw3.h>

namespace fusion {
	class WindowsWindow : public Window
	{
	public:
		WindowsWindow(const WindowProps& props);
		virtual ~WindowsWindow();

		virtual void on_update() override;

		inline virtual unsigned int get_width() const override { return _data.width; }
		inline virtual unsigned int get_height() const override { return _data.height; }

		inline virtual void set_event_callback(const EventCallbackFn& callback) override { _data.event_callback = callback; }
		virtual void set_vsync(bool enabled) override;
		virtual bool is_vsync() const override;

		inline virtual void* get_native_window() const override { return _window; }

	private:
		virtual void init(const WindowProps& props);
		virtual void shutdown();
	private:
		GLFWwindow* _window;

		struct WindowData
		{
			std::string title;
			unsigned int width, height;
			bool vsync;

			EventCallbackFn event_callback;
		};

		WindowData _data;
	};
}