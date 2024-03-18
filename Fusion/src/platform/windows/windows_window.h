#pragma once

#include "fusion/window.h"

#include "fusion/renderer/graphics_context.h"

#include <GLFW/glfw3.h>

namespace Fusion {
	class WindowsWindow : public Window
	{
	public:
		WindowsWindow(const WindowProps& p_props);
		virtual ~WindowsWindow();

		virtual void OnUpdate() override;

		inline virtual unsigned int GetWidth() const override { return _data.width; }
		inline virtual unsigned int GetHeight() const override { return _data.height; }

		inline virtual void SetEventCallback(const EventCallbackFn& p_callback) override { _data.eventCallback = p_callback; }
		virtual void SetVsync(bool p_enabled) override;
		virtual bool IsVsync() const override;

		inline virtual void* GetNativeWindow() const override { return _window; }

	private:
		virtual void Initialize(const WindowProps& p_props);
		virtual void shutdown();
	private:
		GLFWwindow* _window;
		GraphicsContext* _context;

		struct WindowData
		{
			std::string title;
			unsigned int width, height;
			bool vsync;

			EventCallbackFn eventCallback;
		};

		WindowData _data;
	};
}