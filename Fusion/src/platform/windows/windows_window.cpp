#include "fepch.h"
#include "windows_window.h"

#include "fusion/events/application_event.h"
#include "fusion/events/key_event.h"
#include "fusion/events/mouse_event.h"

#include "platform/opengl/opengl_context.h"

namespace fusion {

	static bool _glfw_initialized = false;

	static void glfw_error_callback(int error, const char* description)
	{
		FE_CORE_ERROR("glfw error ({0}): {1}", error, description);
	}

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
			glfwSetErrorCallback(glfw_error_callback);

			_glfw_initialized = true;
		}

		_window = glfwCreateWindow((int)props.width, (int)props.height, _data.title.c_str(), nullptr, nullptr);

		_context = GraphicsContext::create(_window);
		_context->init();

		glfwSetWindowUserPointer(_window, &_data);
		set_vsync(true);

		// set glfw callbacks
		glfwSetWindowSizeCallback(_window, 
			[](GLFWwindow* window, int width, int height)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				data.width = width;
				data.height = height;

				WindowResizeEvent event(width, height);
				data.event_callback(event);
			}
		);

		glfwSetWindowCloseCallback(_window,
			[](GLFWwindow* window)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				WindowCloseEvent event;
				data.event_callback(event);
			}
		);

		glfwSetKeyCallback(_window,
			[](GLFWwindow* window, int key, int scancode, int action, int mods)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				switch (action) {
					case GLFW_PRESS:
					{
						KeyPressedEvent event(key, 0);
						data.event_callback(event);
						break;
					}
					case GLFW_RELEASE:
					{
						KeyReleasedEvent event(key);
						data.event_callback(event);
						break;
					}
					case GLFW_REPEAT:
					{
						KeyPressedEvent event(key, 1);
						data.event_callback(event);
						break;
					}
				}
			}
		);

		glfwSetCharCallback(_window,
			[](GLFWwindow* window, unsigned int keycode)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				KeyTypedEvent event(keycode);
				data.event_callback(event);
			}
		);

		glfwSetMouseButtonCallback(_window,
			[](GLFWwindow* window, int button, int action, int mods)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				switch (action)
				{
					case GLFW_PRESS:
					{
						MouseButtonPressedEvent event(button);
						data.event_callback(event);
						break;
					}
					case GLFW_RELEASE:
					{
						MouseButtonReleasedEvent event(button);
						data.event_callback(event);
						break;
					}
				}
			}
		);

		glfwSetScrollCallback(_window,
			[](GLFWwindow* window, double offset_x, double offset_y)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				MouseScrolledEvent event((float)offset_x, (float)offset_y);
				data.event_callback(event);
			}
		);

		glfwSetCursorPosCallback(_window,
			[](GLFWwindow* window, double pos_x, double pos_y)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				MouseMovedEvent event((float)pos_x, (float)pos_y);
				data.event_callback(event);
			}
		);
	}

	void WindowsWindow::shutdown()
	{
		glfwDestroyWindow(_window);
	}

	void WindowsWindow::on_update()
	{
		glfwPollEvents();
		_context->swap_buffers();
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
