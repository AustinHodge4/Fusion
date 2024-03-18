#pragma once

#include "fusion/input/virtual_key.h"

namespace Fusion {
	class FUSION_API WindowsInput
	{
	public:
		static bool IsKeyPressedImpl(const VirtualKey& p_keycode);
		static bool IsMouseButtonPressedImpl(const VirtualKey& p_button);
		static std::pair<float, float> GetMousePosImpl();
		static float GetMouseXImpl();
		static float GetMouseYImpl();

		static unsigned int GetPlatformKeys(unsigned int* p_keycodes, std::string* p_keynames);
	};
	typedef WindowsInput PlatformInput;
}