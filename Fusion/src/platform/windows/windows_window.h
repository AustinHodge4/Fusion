#pragma once

#include "fusion/window.h"

#include <GLFW/glfw3.h>

namespace fusion {
	class WindowsWindow : public Window
	{
	public:
		WindowsWindow(const WindowProps& props);
		virtual ~WindowsWindow();

		void on_update() override;

		inline unsigned int get_width() const override { return _data.width; }
		inline unsigned int get_height() const override { return _data.height; }

		inline void set_event_callback(const EventCallbackFn& callback) override { _data.event_callback = callback; }
		void set_vsync(bool enabled) override;
		bool is_vsync() const override;

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