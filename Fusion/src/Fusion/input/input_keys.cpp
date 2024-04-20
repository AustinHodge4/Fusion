#include "fepch.h"
#include "input_keys.h"

#include "input.h"

namespace Fusion {

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
	std::vector<Ref<KeyDetails>> Key::_inputkeys;

	void Key::Initialize()
	{
		if (_initialized) return;

		_initialized = true;

		AddKey(KeyDetails(Key::Mouse2D, "Mouse 2D", KeyDetails::MouseButton | KeyDetails::Axis2D));
		AddKey(KeyDetails(Key::MouseX, "Mouse X", KeyDetails::MouseButton | KeyDetails::Axis1D));
		AddKey(KeyDetails(Key::MouseY, "Mouse Y", KeyDetails::MouseButton | KeyDetails::Axis1D));
		AddKey(KeyDetails(Key::MouseScroll, "Mouse Scroll Wheel", KeyDetails::MouseButton | KeyDetails::Axis1D));
		AddKey(KeyDetails(Key::MouseScrollUp, "Mouse Scroll Wheel Up", KeyDetails::MouseButton | KeyDetails::ButtonAxis));
		AddKey(KeyDetails(Key::MouseScrollDown, "Mouse Scroll Wheel Down", KeyDetails::MouseButton | KeyDetails::ButtonAxis));
		
		AddKey(KeyDetails(Key::LeftMouseButton, "Left Mouse Button", KeyDetails::MouseButton));
		AddKey(KeyDetails(Key::RightMouseButton, "Right Mouse Button", KeyDetails::MouseButton));
		AddKey(KeyDetails(Key::MiddleMouseButton, "Middle Mouse Button", KeyDetails::MouseButton));
		AddKey(KeyDetails(Key::ThumbMouseButton, "Thumb Mouse Button", KeyDetails::MouseButton));
		AddKey(KeyDetails(Key::ThumbMouseButton2, "Thumb Mouse Button 2", KeyDetails::MouseButton));
		
		AddKey(KeyDetails(Key::Zero, "0"));
		AddKey(KeyDetails(Key::One, "1"));
		AddKey(KeyDetails(Key::Two, "2"));
		AddKey(KeyDetails(Key::Three, "3"));
		AddKey(KeyDetails(Key::Four, "4"));
		AddKey(KeyDetails(Key::Five, "5"));
		AddKey(KeyDetails(Key::Six, "6"));
		AddKey(KeyDetails(Key::Seven, "7"));
		AddKey(KeyDetails(Key::Eight, "8"));
		AddKey(KeyDetails(Key::Nine, "9"));

		AddKey(KeyDetails(Key::NumPadZero, "Num Pad 0"));
		AddKey(KeyDetails(Key::NumPadOne, "Num Pad 1"));
		AddKey(KeyDetails(Key::NumPadTwo, "Num Pad 2"));
		AddKey(KeyDetails(Key::NumPadThree, "Num Pad 3"));
		AddKey(KeyDetails(Key::NumPadFour, "Num Pad 4"));
		AddKey(KeyDetails(Key::NumPadFive, "Num Pad 5"));
		AddKey(KeyDetails(Key::NumPadSix, "Num Pad 6"));
		AddKey(KeyDetails(Key::NumPadSeven, "Num Pad 7"));
		AddKey(KeyDetails(Key::NumPadEight, "Num Pad 8"));
		AddKey(KeyDetails(Key::NumPadNine, "Num Pad 9"));

		AddKey(KeyDetails(Key::Add, "Add"));
		AddKey(KeyDetails(Key::Subtract, "Subtract"));
		AddKey(KeyDetails(Key::Multiply, "Multiply"));
		AddKey(KeyDetails(Key::Divide, "Divide"));
		AddKey(KeyDetails(Key::Decimal, "Decimal"));
		
		AddKey(KeyDetails(Key::A, "A"));
		AddKey(KeyDetails(Key::B, "B"));
		AddKey(KeyDetails(Key::C, "C"));
		AddKey(KeyDetails(Key::D, "D"));
		AddKey(KeyDetails(Key::E, "E"));
		AddKey(KeyDetails(Key::F, "F"));
		AddKey(KeyDetails(Key::G, "G"));
		AddKey(KeyDetails(Key::H, "H"));
		AddKey(KeyDetails(Key::I, "I"));
		AddKey(KeyDetails(Key::J, "J"));
		AddKey(KeyDetails(Key::K, "K"));
		AddKey(KeyDetails(Key::L, "L"));
		AddKey(KeyDetails(Key::M, "M"));
		AddKey(KeyDetails(Key::N, "N"));
		AddKey(KeyDetails(Key::O, "O"));
		AddKey(KeyDetails(Key::P, "P"));
		AddKey(KeyDetails(Key::Q, "Q"));
		AddKey(KeyDetails(Key::R, "R"));
		AddKey(KeyDetails(Key::S, "S"));
		AddKey(KeyDetails(Key::T, "T"));
		AddKey(KeyDetails(Key::U, "U"));
		AddKey(KeyDetails(Key::V, "V"));
		AddKey(KeyDetails(Key::W, "W"));
		AddKey(KeyDetails(Key::X, "X"));
		AddKey(KeyDetails(Key::Y, "Y"));
		AddKey(KeyDetails(Key::Z, "Z"));
		
		AddKey(KeyDetails(Key::F1, "F1"));
		AddKey(KeyDetails(Key::F2, "F2"));
		AddKey(KeyDetails(Key::F3, "F3"));
		AddKey(KeyDetails(Key::F4, "F4"));
		AddKey(KeyDetails(Key::F5, "F5"));
		AddKey(KeyDetails(Key::F6, "F6"));
		AddKey(KeyDetails(Key::F7, "F7"));
		AddKey(KeyDetails(Key::F8, "F8"));
		AddKey(KeyDetails(Key::F9, "F9"));
		AddKey(KeyDetails(Key::F10, "F10"));
		AddKey(KeyDetails(Key::F11, "F11"));
		AddKey(KeyDetails(Key::F12, "F12"));
		
		AddKey(KeyDetails(Key::Tab, "Tab"));
		AddKey(KeyDetails(Key::Enter, "Enter"));
		AddKey(KeyDetails(Key::SpaceBar, "Space Bar"));
		AddKey(KeyDetails(Key::BackSpace, "Backspace"));
		AddKey(KeyDetails(Key::Escape, "Escape"));

		AddKey(KeyDetails(Key::Left, "Left"));
		AddKey(KeyDetails(Key::Right, "Right"));
		AddKey(KeyDetails(Key::Up, "Up"));
		AddKey(KeyDetails(Key::Down, "Down"));
		
		AddKey(KeyDetails(Key::Insert, "Insert"));
		AddKey(KeyDetails(Key::Delete, "Delete"));
		AddKey(KeyDetails(Key::PageUp, "Page Up"));
		AddKey(KeyDetails(Key::PageDown, "Page Down"));
		AddKey(KeyDetails(Key::Home, "Home"));
		AddKey(KeyDetails(Key::End, "End"));
		
		AddKey(KeyDetails(Key::CapsLock, "Caps Lock"));
		AddKey(KeyDetails(Key::ScrollLock, "Scroll Lock"));
		AddKey(KeyDetails(Key::NumLock, "Num Lock"));
		AddKey(KeyDetails(Key::PrintScreen, "Print Screen"));
		AddKey(KeyDetails(Key::Pause, "Pause"));

		AddKey(KeyDetails(Key::LeftShift, "Left Shift"));
		AddKey(KeyDetails(Key::RightShift, "Right Shift"));
		AddKey(KeyDetails(Key::LeftControl, "Left Control"));
		AddKey(KeyDetails(Key::RightControl, "Right Control"));
		AddKey(KeyDetails(Key::LeftAlt, "Left Alt"));
		AddKey(KeyDetails(Key::RightAlt, "Right Alt"));
		AddKey(KeyDetails(Key::LeftCommand, "Left Command"));
		AddKey(KeyDetails(Key::RightCommand, "Right Command"));
		
		AddKey(KeyDetails(Key::Semicolon, "Semicolon"));
		AddKey(KeyDetails(Key::Apostrophe, "Apostrophe"));
		AddKey(KeyDetails(Key::Comma, "Comma"));
		AddKey(KeyDetails(Key::Period, "Period"));
		AddKey(KeyDetails(Key::Hyphen, "Hyphen"));
		AddKey(KeyDetails(Key::Slash, "Slash"));
		AddKey(KeyDetails(Key::BackSlash, "Backslash"));
		AddKey(KeyDetails(Key::Equals, "Equals"));
		AddKey(KeyDetails(Key::LeftBracket, "Left Bracket"));
		AddKey(KeyDetails(Key::RightBracket, "Right Bracket"));
		AddKey(KeyDetails(Key::GraveAccent, "Accent"));
		
		AddKey(KeyDetails(Key::Colon, "Colon"));
		AddKey(KeyDetails(Key::Quote, "Quote"));
		AddKey(KeyDetails(Key::Underscore, "Underscore"));
		AddKey(KeyDetails(Key::LeftParantheses, "Left Parantheses"));
		AddKey(KeyDetails(Key::RightParantheses, "Right Parantheses"));
		AddKey(KeyDetails(Key::Ampersand, "Ampersand"));
		AddKey(KeyDetails(Key::Asterix, "Asterix"));
		AddKey(KeyDetails(Key::Caret, "Caret"));
		AddKey(KeyDetails(Key::Dollar, "Dollar"));
		AddKey(KeyDetails(Key::Exclamation, "Exclamation"));

		// init platform keys
		Input::InitializeKeys();
	}

	void Key::AddKey(KeyDetails& p_keydetails)
	{
		VirtualKey& key = p_keydetails.GetKey();
		key._keydetails = CreateRef<KeyDetails>(p_keydetails);
		_inputkeys.push_back(key._keydetails);
	}

	const Ref<KeyDetails> Key::GetKeyDetails(const VirtualKey& p_key)
	{
		auto entry = std::find_if(_inputkeys.begin(), _inputkeys.end(), [&](Ref<KeyDetails> key) { return key->GetKey() == p_key; });
		if (entry == _inputkeys.end())
		{
			FE_CORE_ASSERT(false, "VirtualKey is not registered (has no key details)!");
			return nullptr;
		}
		return *entry;
	}
}