#pragma once

#include "input_keys.h"

namespace Fusion {

	class KeyEvent;

	class PlatformInput
	{
	public:
		static bool IsKeyPressedImpl(const VirtualKey& p_keycode);
		static bool IsMouseButtonPressedImpl(const VirtualKey& p_button);
		static std::pair<float, float> GetMousePosImpl();
		static float GetMouseXImpl();
		static float GetMouseYImpl();

		static unsigned int GetPlatformKeys(unsigned int* p_keycodes, std::string* p_keynames);
	};

	class FUSION_API Input 
	{
	public:
		friend PlatformInput;
		friend Key;
		friend KeyEvent;

		inline static bool IsKeyPressed(const VirtualKey& p_keycode) { return PlatformInput::IsKeyPressedImpl(p_keycode); }
		inline static bool IsMouseButtonPressed(const VirtualKey& p_button) { return PlatformInput::IsMouseButtonPressedImpl(p_button); }
		inline static float GetMouseX() { return PlatformInput::GetMouseXImpl(); }
		inline static float GetMouseY() { return PlatformInput::GetMouseYImpl(); }
		inline static std::pair<float, float> GetMousePos() { return PlatformInput::GetMousePosImpl(); }
	protected:
		static const VirtualKey& GetKeyFromCodes(const int p_keycode);
		static int GetCodesFromKey(const VirtualKey& p_key);

		static void InitializeKeys();

	private:
		static std::unordered_map<int, VirtualKey> _keymap;
	};


}