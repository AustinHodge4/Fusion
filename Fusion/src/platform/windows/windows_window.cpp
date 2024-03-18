#include "fepch.h"
#include "windows_window.h"

#include "fusion/events/application_event.h"
#include "fusion/events/key_event.h"
#include "fusion/events/mouse_event.h"

#include "platform/opengl/opengl_context.h"

namespace Fusion {

	static bool _glfwInitialized = false;

	static void glfwErrorCallback(int p_error, const char* p_description)
	{
		FE_CORE_ERROR("glfw error ({0}): {1}", p_error, p_description);
	}

	WindowsWindow::WindowsWindow(const WindowProps& p_props)
	{
		Initialize(p_props);
	}

	WindowsWindow::~WindowsWindow() 
	{
		shutdown();
	}

	void WindowsWindow::Initialize(const WindowProps& p_props)
	{
		_data.title = p_props.title;
		_data.width = p_props.width;
		_data.height = p_props.height;

		FE_CORE_INFO("Creating window {0} ({1}, {2})", p_props.title, p_props.width, p_props.height);

		if (!_glfwInitialized) {
			int success = glfwInit();
			FE_CORE_ASSERT(success, "Could not init glfw!");
			glfwSetErrorCallback(glfwErrorCallback);

			_glfwInitialized = true;
		}

		_window = glfwCreateWindow((int)p_props.width, (int)p_props.height, _data.title.c_str(), nullptr, nullptr);

		_context = GraphicsContext::Create(_window);
		_context->Initialize();

		glfwSetWindowUserPointer(_window, &_data);
		SetVsync(true);

		// set glfw callbacks
		glfwSetWindowSizeCallback(_window, 
			[](GLFWwindow* p_window, int p_width, int p_height)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(p_window);
				data.width = p_width;
				data.height = p_height;

				WindowResizeEvent event(p_width, p_height);
				data.eventCallback(event);
			}
		);

		glfwSetWindowCloseCallback(_window,
			[](GLFWwindow* p_window)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(p_window);

				WindowCloseEvent event;
				data.eventCallback(event);
			}
		);

		glfwSetKeyCallback(_window,
			[](GLFWwindow* p_window, int p_key, int p_scancode, int p_action, int p_mods)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(p_window);

				switch (p_action) {
					case GLFW_PRESS:
					{
						KeyPressedEvent event(p_key, 0);
						data.eventCallback(event);
						break;
					}
					case GLFW_RELEASE:
					{
						KeyReleasedEvent event(p_key);
						data.eventCallback(event);
						break;
					}
					case GLFW_REPEAT:
					{
						KeyPressedEvent event(p_key, 1);
						data.eventCallback(event);
						break;
					}
				}
			}
		);

		glfwSetCharCallback(_window,
			[](GLFWwindow* p_window, unsigned int p_keycode)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(p_window);

				KeyTypedEvent event(p_keycode);
				data.eventCallback(event);
			}
		);

		glfwSetMouseButtonCallback(_window,
			[](GLFWwindow* p_window, int p_button, int p_action, int p_mods)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(p_window);

				switch (p_action)
				{
					case GLFW_PRESS:
					{
						MouseButtonPressedEvent event(p_button);
						data.eventCallback(event);
						break;
					}
					case GLFW_RELEASE:
					{
						MouseButtonReleasedEvent event(p_button);
						data.eventCallback(event);
						break;
					}
				}
			}
		);

		glfwSetScrollCallback(_window,
			[](GLFWwindow* p_window, double p_offsetX, double p_offsetY)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(p_window);

				MouseScrolledEvent event((float)p_offsetX, (float)p_offsetY);
				data.eventCallback(event);
			}
		);

		glfwSetCursorPosCallback(_window,
			[](GLFWwindow* p_window, double p_posX, double p_posY)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(p_window);

				MouseMovedEvent event((float)p_posX, (float)p_posY);
				data.eventCallback(event);
			}
		);
	}

	void WindowsWindow::shutdown()
	{
		glfwDestroyWindow(_window);
	}

	void WindowsWindow::OnUpdate()
	{
		glfwPollEvents();
		_context->SwapBuffers();
	}

	void WindowsWindow::SetVsync(bool p_enabled)
	{
		if (p_enabled) {
			glfwSwapInterval(1);
		}
		else {
			glfwSwapInterval(0);
		}

		_data.vsync = p_enabled;
	}

	bool WindowsWindow::IsVsync() const
	{
		return _data.vsync;
	}
}
