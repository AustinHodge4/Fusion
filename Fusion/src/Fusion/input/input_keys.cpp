#include "fepch.h"
#include "input_keys.h"

#include "input.h"
#include "platform/platform_input_manager.h"

namespace fusion {

	const VirtualKey Key::Invalid("Invalid");

	const VirtualKey Key::Mouse2D("Mouse2D");
	const VirtualKey Key::MouseX("MouseX");
	const VirtualKey Key::MouseY("MouseY");
	const VirtualKey Key::MouseScroll("MouseScroll");
	const VirtualKey Key::MouseScrollUp("MouseScrollUp");
	const VirtualKey Key::MouseScrollDown("MouseScrollDown");

	const VirtualKey Key::LeftMouseButton("LeftMouseButton");
	const VirtualKey Key::RightMouseButton("RightMouseButton");
	const VirtualKey Key::MiddleMouseButton("MiddleMouseButton");
	const VirtualKey Key::ThumbMouseButton("ThumbMouseButton");
	const VirtualKey Key::ThumbMouseButton2("ThumbMouseButton2");

	const VirtualKey Key::Zero("0");
	const VirtualKey Key::One("1");
	const VirtualKey Key::Two("2");
	const VirtualKey Key::Three("3");
	const VirtualKey Key::Four("4");
	const VirtualKey Key::Five("5");
	const VirtualKey Key::Six("6");
	const VirtualKey Key::Seven("7");
	const VirtualKey Key::Eight("8");
	const VirtualKey Key::Nine("9");

	const VirtualKey Key::NumPadZero("NumPadZero");
	const VirtualKey Key::NumPadOne("NumPadOne");
	const VirtualKey Key::NumPadTwo("NumPadTwo");
	const VirtualKey Key::NumPadThree("NumPadThree");
	const VirtualKey Key::NumPadFour("NumPadFour");
	const VirtualKey Key::NumPadFive("NumPadFive");
	const VirtualKey Key::NumPadSix("NumPadSix");
	const VirtualKey Key::NumPadSeven("NumPadSeven");
	const VirtualKey Key::NumPadEight("NumPadEight");
	const VirtualKey Key::NumPadNine("NumPadNine");

	const VirtualKey Key::Add("Add");
	const VirtualKey Key::Subtract("Subtract");
	const VirtualKey Key::Multiply("Multiply");
	const VirtualKey Key::Divide("Divide");
	const VirtualKey Key::Decimal("Decimal");
	
	const VirtualKey Key::A("A");
	const VirtualKey Key::B("B");
	const VirtualKey Key::C("C");
	const VirtualKey Key::D("D");
	const VirtualKey Key::E("E");
	const VirtualKey Key::F("F");
	const VirtualKey Key::G("G");
	const VirtualKey Key::H("H");
	const VirtualKey Key::I("I");
	const VirtualKey Key::J("J");
	const VirtualKey Key::K("K");
	const VirtualKey Key::L("L");
	const VirtualKey Key::M("M");
	const VirtualKey Key::N("N");
	const VirtualKey Key::O("O");
	const VirtualKey Key::P("P");
	const VirtualKey Key::Q("Q");
	const VirtualKey Key::R("R");
	const VirtualKey Key::S("S");
	const VirtualKey Key::T("T");
	const VirtualKey Key::U("U");
	const VirtualKey Key::V("V");
	const VirtualKey Key::W("W");
	const VirtualKey Key::X("X");
	const VirtualKey Key::Y("Y");
	const VirtualKey Key::Z("Z");
	
	const VirtualKey Key::F1("F1");
	const VirtualKey Key::F2("F2");
	const VirtualKey Key::F3("F3");
	const VirtualKey Key::F4("F4");
	const VirtualKey Key::F5("F5");
	const VirtualKey Key::F6("F6");
	const VirtualKey Key::F7("F7");
	const VirtualKey Key::F8("F8");
	const VirtualKey Key::F9("F9");
	const VirtualKey Key::F10("F10");
	const VirtualKey Key::F11("F11");
	const VirtualKey Key::F12("F12");
	
	const VirtualKey Key::Tab("Tab");
	const VirtualKey Key::Enter("Enter");
	const VirtualKey Key::SpaceBar("SpaceBar");
	const VirtualKey Key::BackSpace("BackSpace");
	const VirtualKey Key::Escape("Escape");
	
	const VirtualKey Key::Left("Left");
	const VirtualKey Key::Right("Right");
	const VirtualKey Key::Up("Up");
	const VirtualKey Key::Down("Down");
	
	const VirtualKey Key::Insert("Insert");
	const VirtualKey Key::Delete("Delete");
	const VirtualKey Key::PageUp("PageUp");
	const VirtualKey Key::PageDown("PageDown");
	const VirtualKey Key::Home("Home");
	const VirtualKey Key::End("End");
	
	const VirtualKey Key::CapsLock("CapsLock");
	const VirtualKey Key::ScrollLock("ScrollLock");
	const VirtualKey Key::NumLock("NumLock");
	const VirtualKey Key::PrintScreen("PrintScreen");
	const VirtualKey Key::Pause("Pause");
	
	const VirtualKey Key::LeftShift("LeftShift");
	const VirtualKey Key::RightShift("RightShift");
	const VirtualKey Key::LeftControl("LeftControl");
	const VirtualKey Key::RightControl("RightControl");
	const VirtualKey Key::LeftAlt("LeftAlt");
	const VirtualKey Key::RightAlt("RightAlt");
	const VirtualKey Key::LeftCommand("LeftCommand");
	const VirtualKey Key::RightCommand("RightCommand");
	
	const VirtualKey Key::Semicolon("Semicolon");
	const VirtualKey Key::Apostrophe("Apostrophe");
	const VirtualKey Key::Comma("Comma");
	const VirtualKey Key::Period("Period");
	const VirtualKey Key::Hyphen("Hyphen");
	const VirtualKey Key::Slash("Slash");
	const VirtualKey Key::BackSlash("BackSlash");
	const VirtualKey Key::Equals("Equals");
	const VirtualKey Key::LeftBracket("LeftBracket");
	const VirtualKey Key::RightBracket("RightBracket");
	const VirtualKey Key::GraveAccent("GraveAccent");
	
	const VirtualKey Key::Colon("Colon");
	const VirtualKey Key::Quote("Quote");
	const VirtualKey Key::Underscore("Underscore");
	const VirtualKey Key::LeftParantheses("LeftParantheses");
	const VirtualKey Key::RightParantheses("RightParantheses");
	const VirtualKey Key::Ampersand("Ampersand");
	const VirtualKey Key::Asterix("Asterix");
	const VirtualKey Key::Caret("Caret");
	const VirtualKey Key::Dollar("Dollar");
	const VirtualKey Key::Exclamation("Exclamation");
	
	bool Key::_initialized = false;
	std::map<VirtualKey, std::shared_ptr<KeyDetails>> Key::_input_keys;

	void Key::initialize()
	{
		if (_initialized) return;

		_initialized = true;

		add_key(KeyDetails(Key::Mouse2D, "Mouse 2D", KeyDetails::MouseButton | KeyDetails::Axis2D));
		add_key(KeyDetails(Key::MouseX, "Mouse X", KeyDetails::MouseButton | KeyDetails::Axis1D));
		add_key(KeyDetails(Key::MouseY, "Mouse Y", KeyDetails::MouseButton | KeyDetails::Axis1D));
		add_key(KeyDetails(Key::MouseScroll, "Mouse Scroll Wheel", KeyDetails::MouseButton | KeyDetails::Axis1D));
		add_key(KeyDetails(Key::MouseScrollUp, "Mouse Scroll Wheel Up", KeyDetails::MouseButton | KeyDetails::ButtonAxis));
		add_key(KeyDetails(Key::MouseScrollDown, "Mouse Scroll Wheel Down", KeyDetails::MouseButton | KeyDetails::ButtonAxis));
		
		add_key(KeyDetails(Key::LeftMouseButton, "Left Mouse Button", KeyDetails::MouseButton));
		add_key(KeyDetails(Key::RightMouseButton, "Right Mouse Button", KeyDetails::MouseButton));
		add_key(KeyDetails(Key::MiddleMouseButton, "Middle Mouse Button", KeyDetails::MouseButton));
		add_key(KeyDetails(Key::ThumbMouseButton, "Thumb Mouse Button", KeyDetails::MouseButton));
		add_key(KeyDetails(Key::ThumbMouseButton2, "Thumb Mouse Button 2", KeyDetails::MouseButton));
		
		add_key(KeyDetails(Key::Zero, "0"));
		add_key(KeyDetails(Key::One, "1"));
		add_key(KeyDetails(Key::Two, "2"));
		add_key(KeyDetails(Key::Three, "3"));
		add_key(KeyDetails(Key::Four, "4"));
		add_key(KeyDetails(Key::Five, "5"));
		add_key(KeyDetails(Key::Six, "6"));
		add_key(KeyDetails(Key::Seven, "7"));
		add_key(KeyDetails(Key::Eight, "8"));
		add_key(KeyDetails(Key::Nine, "9"));

		add_key(KeyDetails(Key::NumPadZero, "Num Pad 0"));
		add_key(KeyDetails(Key::NumPadOne, "Num Pad 1"));
		add_key(KeyDetails(Key::NumPadTwo, "Num Pad 2"));
		add_key(KeyDetails(Key::NumPadThree, "Num Pad 3"));
		add_key(KeyDetails(Key::NumPadFour, "Num Pad 4"));
		add_key(KeyDetails(Key::NumPadFive, "Num Pad 5"));
		add_key(KeyDetails(Key::NumPadSix, "Num Pad 6"));
		add_key(KeyDetails(Key::NumPadSeven, "Num Pad 7"));
		add_key(KeyDetails(Key::NumPadEight, "Num Pad 8"));
		add_key(KeyDetails(Key::NumPadNine, "Num Pad 9"));

		add_key(KeyDetails(Key::Add, "Add"));
		add_key(KeyDetails(Key::Subtract, "Subtract"));
		add_key(KeyDetails(Key::Multiply, "Multiply"));
		add_key(KeyDetails(Key::Divide, "Divide"));
		add_key(KeyDetails(Key::Decimal, "Decimal"));
		
		add_key(KeyDetails(Key::A, "A"));
		add_key(KeyDetails(Key::B, "B"));
		add_key(KeyDetails(Key::C, "C"));
		add_key(KeyDetails(Key::D, "D"));
		add_key(KeyDetails(Key::E, "E"));
		add_key(KeyDetails(Key::F, "F"));
		add_key(KeyDetails(Key::G, "G"));
		add_key(KeyDetails(Key::H, "H"));
		add_key(KeyDetails(Key::I, "I"));
		add_key(KeyDetails(Key::J, "J"));
		add_key(KeyDetails(Key::K, "K"));
		add_key(KeyDetails(Key::L, "L"));
		add_key(KeyDetails(Key::M, "M"));
		add_key(KeyDetails(Key::N, "N"));
		add_key(KeyDetails(Key::O, "O"));
		add_key(KeyDetails(Key::P, "P"));
		add_key(KeyDetails(Key::Q, "Q"));
		add_key(KeyDetails(Key::R, "R"));
		add_key(KeyDetails(Key::S, "S"));
		add_key(KeyDetails(Key::T, "T"));
		add_key(KeyDetails(Key::U, "U"));
		add_key(KeyDetails(Key::V, "V"));
		add_key(KeyDetails(Key::W, "W"));
		add_key(KeyDetails(Key::X, "X"));
		add_key(KeyDetails(Key::Y, "Y"));
		add_key(KeyDetails(Key::Z, "Z"));
		
		add_key(KeyDetails(Key::F1, "F1"));
		add_key(KeyDetails(Key::F2, "F2"));
		add_key(KeyDetails(Key::F3, "F3"));
		add_key(KeyDetails(Key::F4, "F4"));
		add_key(KeyDetails(Key::F5, "F5"));
		add_key(KeyDetails(Key::F6, "F6"));
		add_key(KeyDetails(Key::F7, "F7"));
		add_key(KeyDetails(Key::F8, "F8"));
		add_key(KeyDetails(Key::F9, "F9"));
		add_key(KeyDetails(Key::F10, "F10"));
		add_key(KeyDetails(Key::F11, "F11"));
		add_key(KeyDetails(Key::F12, "F12"));
		
		add_key(KeyDetails(Key::Tab, "Tab"));
		add_key(KeyDetails(Key::Enter, "Enter"));
		add_key(KeyDetails(Key::SpaceBar, "Space Bar"));
		add_key(KeyDetails(Key::BackSpace, "Backspace"));
		add_key(KeyDetails(Key::Escape, "Escape"));

		add_key(KeyDetails(Key::Left, "Left"));
		add_key(KeyDetails(Key::Right, "Right"));
		add_key(KeyDetails(Key::Up, "Up"));
		add_key(KeyDetails(Key::Down, "Down"));
		
		add_key(KeyDetails(Key::Insert, "Insert"));
		add_key(KeyDetails(Key::Delete, "Delete"));
		add_key(KeyDetails(Key::PageUp, "Page Up"));
		add_key(KeyDetails(Key::PageDown, "Page Down"));
		add_key(KeyDetails(Key::Home, "Home"));
		add_key(KeyDetails(Key::End, "End"));
		
		add_key(KeyDetails(Key::CapsLock, "Caps Lock"));
		add_key(KeyDetails(Key::ScrollLock, "Scroll Lock"));
		add_key(KeyDetails(Key::NumLock, "Num Lock"));
		add_key(KeyDetails(Key::PrintScreen, "Print Screen"));
		add_key(KeyDetails(Key::Pause, "Pause"));

		add_key(KeyDetails(Key::LeftShift, "Left Shift"));
		add_key(KeyDetails(Key::RightShift, "Right Shift"));
		add_key(KeyDetails(Key::LeftControl, "Left Control"));
		add_key(KeyDetails(Key::RightControl, "Right Control"));
		add_key(KeyDetails(Key::LeftAlt, "Left Alt"));
		add_key(KeyDetails(Key::RightAlt, "Right Alt"));
		add_key(KeyDetails(Key::LeftCommand, "Left Command"));
		add_key(KeyDetails(Key::RightCommand, "Right Command"));
		
		add_key(KeyDetails(Key::Semicolon, "Semicolon"));
		add_key(KeyDetails(Key::Apostrophe, "Apostrophe"));
		add_key(KeyDetails(Key::Comma, "Comma"));
		add_key(KeyDetails(Key::Period, "Period"));
		add_key(KeyDetails(Key::Hyphen, "Hyphen"));
		add_key(KeyDetails(Key::Slash, "Slash"));
		add_key(KeyDetails(Key::BackSlash, "Backslash"));
		add_key(KeyDetails(Key::Equals, "Equals"));
		add_key(KeyDetails(Key::LeftBracket, "Left Bracket"));
		add_key(KeyDetails(Key::RightBracket, "Right Bracket"));
		add_key(KeyDetails(Key::GraveAccent, "Accent"));
		
		add_key(KeyDetails(Key::Colon, "Colon"));
		add_key(KeyDetails(Key::Quote, "Quote"));
		add_key(KeyDetails(Key::Underscore, "Underscore"));
		add_key(KeyDetails(Key::LeftParantheses, "Left Parantheses"));
		add_key(KeyDetails(Key::RightParantheses, "Right Parantheses"));
		add_key(KeyDetails(Key::Ampersand, "Ampersand"));
		add_key(KeyDetails(Key::Asterix, "Asterix"));
		add_key(KeyDetails(Key::Caret, "Caret"));
		add_key(KeyDetails(Key::Dollar, "Dollar"));
		add_key(KeyDetails(Key::Exclamation, "Exclamation"));

		// init platform keys
		Input::initialize_keys();
	}

	void Key::add_key(KeyDetails& key_details)
	{
		VirtualKey& key = key_details.get_key();
		key._key_details = std::make_shared<KeyDetails>(key_details);
		_input_keys.insert({ key, key._key_details});
	}

	std::shared_ptr<KeyDetails> Key::get_key_details(const VirtualKey key)
	{
		auto entry = _input_keys.find(key);
		if (entry == _input_keys.end())
		{
			FE_CORE_ERROR("VirtualKey is not registered (has no key details)!");
			return std::shared_ptr<KeyDetails>();
		}
		return entry->second;
	}
}