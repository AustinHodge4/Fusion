#include "fepch.h"
#include "window.h"

#include "platform/windows/windows_window.h"

namespace Fusion {

	Window* Window::Create(const WindowProps& p_props)
	{
		return new WindowsWindow(p_props);
	}
}