#include "fepch.h"
#include "window.h"

#ifdef FE_PLATFORM_WINDOWS
#include "platform/windows/windows_window.h"
#endif

namespace Fusion {

	Window* Window::Create(const WindowProps& p_props)
	{
#ifdef FE_PLATFORM_WINDOWS
		return new WindowsWindow(p_props);
#else
		FE_CORE_ASSERT(false, "Can't create window! unknown platform!!");
		return nullptr;
#endif
	}
}