#include "fepch.h"
#include "windows_window.h"

namespace fusion {

	static bool _glfw_initialized = false;

	Window* Window::create(const WindowProps& props)
	{
		return new WindowsWindow(props);
	}

	WindowsWindow::WindowsWindow(const WindowProps& props)
	{
		init(props);
	}

	WindowsWindow::~WindowsWindow() 
	{
		shutdown();
	}

	void WindowsWindow::init(const WindowProps& props)
	{
		_data.title = props.title;
		_data.width = props.width;
		_data.height = props.height;

		FE_CORE_INFO("Creating window {0} ({1}, {2})", props.title, props.width, props.height);

		if (!_glfw_initialized) {
			int success = glfwInit();
			FE_CORE_ASSERT(success, "Could not init glfw!");
			_glfw_initialized = true;
		}

		_window = glfwCreateWindow((int)props.width, (int)props.height, _data.title.c_str(), nullptr, nullptr);
		glfwMakeContextCurrent(_window);
		glfwSetWindowUserPointer(_window, &_data);
		set_vsync(true);
	}

	void WindowsWindow::shutdown()
	{
		glfwDestroyWindow(_window);
	}

	void WindowsWindow::on_update()
	{
		glfwPollEvents();
		glfwSwapBuffers(_window);
	}

	void WindowsWindow::set_vsync(bool enabled)
	{
		if (enabled) {
			glfwSwapInterval(1);
		}
		else {
			glfwSwapInterval(0);
		}

		_data.vsync = enabled;
	}

	bool WindowsWindow::is_vsync() const
	{
		return _data.vsync;
	}

}
