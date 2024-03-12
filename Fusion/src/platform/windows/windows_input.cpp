#include "fepch.h"
#include "windows_input.h"

#include "fusion/application.h"
#include "fusion/input/input.h"

#include <GLFW/glfw3.h>

namespace fusion {

	bool WindowsInput::is_key_pressed_impl(const VirtualKey& keycode)
	{
		auto window = static_cast<GLFWwindow*>(Application::get().get_window().get_native_window());
		auto state = glfwGetKey(window, Input::get_codes_from_key(keycode));
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}
	bool WindowsInput::is_mouse_button_pressed_impl(const VirtualKey& button)
	{
		auto window = static_cast<GLFWwindow*>(Application::get().get_window().get_native_window());
		auto state = glfwGetMouseButton(window, Input::get_codes_from_key(button));
		return state == GLFW_PRESS;
	}
	std::pair<float, float> WindowsInput::get_mouse_pos_impl()
	{
		auto window = static_cast<GLFWwindow*>(Application::get().get_window().get_native_window());
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		return { (float)xpos, (float)ypos };
	}

	float WindowsInput::get_mouse_x_impl()
	{
		auto [x, y] = get_mouse_pos_impl();
		return x;
	}

	float WindowsInput::get_mouse_y_impl()
	{
		auto [x, y] = get_mouse_pos_impl();
		return y;
	}

	unsigned int WindowsInput::get_platform_keys(unsigned int* keycodes, std::string* key_names)
	{
	#define ADDKEY(code, name) if (number_of_mappings<256) { keycodes[number_of_mappings] = code; key_names[number_of_mappings] = name; ++number_of_mappings;};
		
		unsigned int number_of_mappings = 0;

		ADDKEY(GLFW_MOUSE_BUTTON_LEFT, "LeftMouseButton");
		ADDKEY(GLFW_MOUSE_BUTTON_RIGHT, "RightMouseButton");
		ADDKEY(GLFW_MOUSE_BUTTON_MIDDLE, "MiddleMouseButton");
		ADDKEY(GLFW_MOUSE_BUTTON_LAST, "ThumbMouseButton");
		ADDKEY(GLFW_MOUSE_BUTTON_5, "ThumbMouseButton2");

		ADDKEY(GLFW_KEY_0, "0");
		ADDKEY(GLFW_KEY_1, "1");
		ADDKEY(GLFW_KEY_2, "2");
		ADDKEY(GLFW_KEY_3, "3");
		ADDKEY(GLFW_KEY_4, "4");
		ADDKEY(GLFW_KEY_5, "5");
		ADDKEY(GLFW_KEY_6, "6");
		ADDKEY(GLFW_KEY_7, "7");
		ADDKEY(GLFW_KEY_8, "8");
		ADDKEY(GLFW_KEY_9, "9");

		ADDKEY(GLFW_KEY_KP_0, "NumPadZero");
		ADDKEY(GLFW_KEY_KP_1, "NumPadOne");
		ADDKEY(GLFW_KEY_KP_2, "NumPadTwo");
		ADDKEY(GLFW_KEY_KP_3, "NumPadThree");
		ADDKEY(GLFW_KEY_KP_4, "NumPadFour");
		ADDKEY(GLFW_KEY_KP_5, "NumPadFive");
		ADDKEY(GLFW_KEY_KP_6, "NumPadSix");
		ADDKEY(GLFW_KEY_KP_7, "NumPadSeven");
		ADDKEY(GLFW_KEY_KP_8, "NumPadEight");
		ADDKEY(GLFW_KEY_KP_9, "NumPadNine");

		ADDKEY(GLFW_KEY_KP_MULTIPLY, "Multiply");
		ADDKEY(GLFW_KEY_KP_ADD, "Add");
		ADDKEY(GLFW_KEY_KP_SUBTRACT, "Subtract");
		ADDKEY(GLFW_KEY_KP_DIVIDE, "Divide");
		ADDKEY(GLFW_KEY_KP_DECIMAL, "Decimal");

		ADDKEY(GLFW_KEY_A, "A");
		ADDKEY(GLFW_KEY_B, "B");
		ADDKEY(GLFW_KEY_C, "C");
		ADDKEY(GLFW_KEY_D, "D");
		ADDKEY(GLFW_KEY_E, "E");
		ADDKEY(GLFW_KEY_F, "F");
		ADDKEY(GLFW_KEY_G, "G");
		ADDKEY(GLFW_KEY_H, "H");
		ADDKEY(GLFW_KEY_I, "I");
		ADDKEY(GLFW_KEY_J, "J");
		ADDKEY(GLFW_KEY_K, "K");
		ADDKEY(GLFW_KEY_L, "L");
		ADDKEY(GLFW_KEY_M, "M");
		ADDKEY(GLFW_KEY_N, "N");
		ADDKEY(GLFW_KEY_O, "O");
		ADDKEY(GLFW_KEY_P, "P");
		ADDKEY(GLFW_KEY_Q, "Q");
		ADDKEY(GLFW_KEY_R, "R");
		ADDKEY(GLFW_KEY_S, "S");
		ADDKEY(GLFW_KEY_T, "T");
		ADDKEY(GLFW_KEY_U, "U");
		ADDKEY(GLFW_KEY_V, "V");
		ADDKEY(GLFW_KEY_W, "W");
		ADDKEY(GLFW_KEY_X, "X");
		ADDKEY(GLFW_KEY_Y, "Y");
		ADDKEY(GLFW_KEY_Z, "Z");

		ADDKEY(GLFW_KEY_F1, "F1");
		ADDKEY(GLFW_KEY_F2, "F2");
		ADDKEY(GLFW_KEY_F3, "F3");
		ADDKEY(GLFW_KEY_F4, "F4");
		ADDKEY(GLFW_KEY_F5, "F5");
		ADDKEY(GLFW_KEY_F6, "F6");
		ADDKEY(GLFW_KEY_F7, "F7");
		ADDKEY(GLFW_KEY_F8, "F8");
		ADDKEY(GLFW_KEY_F9, "F9");
		ADDKEY(GLFW_KEY_F10, "F10");
		ADDKEY(GLFW_KEY_F11, "F11");
		ADDKEY(GLFW_KEY_F12, "F12");

		ADDKEY(GLFW_KEY_TAB, "Tab");
		ADDKEY(GLFW_KEY_ENTER, "Enter");
		ADDKEY(GLFW_KEY_SPACE, "SpaceBar");
		ADDKEY(GLFW_KEY_BACKSPACE, "BackSpace");
		ADDKEY(GLFW_KEY_ESCAPE, "Escape");

		ADDKEY(GLFW_KEY_LEFT, "Left");
		ADDKEY(GLFW_KEY_RIGHT, "Right");
		ADDKEY(GLFW_KEY_UP, "Up");
		ADDKEY(GLFW_KEY_DOWN, "Down");

		ADDKEY(GLFW_KEY_INSERT, "Insert");
		ADDKEY(GLFW_KEY_DELETE, "Delete");
		ADDKEY(GLFW_KEY_PAGE_UP, "PageUp");
		ADDKEY(GLFW_KEY_PAGE_DOWN, "PageDown");
		ADDKEY(GLFW_KEY_HOME, "Home");
		ADDKEY(GLFW_KEY_END, "End");

		ADDKEY(GLFW_KEY_CAPS_LOCK, "CapsLock");
		ADDKEY(GLFW_KEY_SCROLL_LOCK, "ScrollLock");
		ADDKEY(GLFW_KEY_NUM_LOCK, "NumLock");
		ADDKEY(GLFW_KEY_PRINT_SCREEN, "PrintScreen");
		ADDKEY(GLFW_KEY_PAUSE, "Pause");

		ADDKEY(GLFW_KEY_LEFT_SHIFT, "LeftShift");
		ADDKEY(GLFW_KEY_RIGHT_SHIFT, "RightShift");
		ADDKEY(GLFW_KEY_LEFT_CONTROL, "LeftControl");
		ADDKEY(GLFW_KEY_RIGHT_CONTROL, "RightControl");
		ADDKEY(GLFW_KEY_LEFT_ALT, "LeftAlt");
		ADDKEY(GLFW_KEY_RIGHT_ALT, "RightAlt");
		ADDKEY(GLFW_KEY_LEFT_SUPER, "LeftCommand");
		ADDKEY(GLFW_KEY_RIGHT_SUPER, "RightCommand");
		
		ADDKEY(GLFW_KEY_SEMICOLON, "Semicolon");
		ADDKEY(GLFW_KEY_APOSTROPHE, "Apostrophe");
		ADDKEY(GLFW_KEY_COMMA, "Comma");
		ADDKEY(GLFW_KEY_PERIOD, "Period");
		ADDKEY(GLFW_KEY_MINUS, "Hyphen");
		ADDKEY(GLFW_KEY_SLASH, "Slash");
		ADDKEY(GLFW_KEY_BACKSLASH, "BackSlash");
		ADDKEY(GLFW_KEY_EQUAL, "Equals");
		ADDKEY(GLFW_KEY_LEFT_BRACKET, "LeftBracket");
		ADDKEY(GLFW_KEY_RIGHT_BRACKET, "RightBracket");
		ADDKEY(GLFW_KEY_GRAVE_ACCENT, "GraveAccent");

		FE_CORE_INFO("Added Windows platform keys...");

		return number_of_mappings;
	}
}