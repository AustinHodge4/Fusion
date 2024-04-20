#pragma once

#include "fusion/input/virtual_key.h"
#include "fusion/input/key_details.h"

#include <map>

namespace Fusion {

	class FUSION_API Key 
	{
	public:
		static const VirtualKey Invalid;

		static const VirtualKey Mouse2D;
		static const VirtualKey MouseX;
		static const VirtualKey MouseY;
		static const VirtualKey MouseScroll;
		static const VirtualKey MouseScrollUp;
		static const VirtualKey MouseScrollDown;

		static const VirtualKey LeftMouseButton;
		static const VirtualKey RightMouseButton;
		static const VirtualKey MiddleMouseButton;
		static const VirtualKey ThumbMouseButton;
		static const VirtualKey ThumbMouseButton2;

		static const VirtualKey Zero;
		static const VirtualKey One;
		static const VirtualKey Two;
		static const VirtualKey Three;
		static const VirtualKey Four;
		static const VirtualKey Five;
		static const VirtualKey Six;
		static const VirtualKey Seven;
		static const VirtualKey Eight;
		static const VirtualKey Nine;

		static const VirtualKey NumPadZero;
		static const VirtualKey NumPadOne;
		static const VirtualKey NumPadTwo;
		static const VirtualKey NumPadThree;
		static const VirtualKey NumPadFour;
		static const VirtualKey NumPadFive;
		static const VirtualKey NumPadSix;
		static const VirtualKey NumPadSeven;
		static const VirtualKey NumPadEight;
		static const VirtualKey NumPadNine;

		static const VirtualKey Add;
		static const VirtualKey Subtract;
		static const VirtualKey Multiply;
		static const VirtualKey Divide;
		static const VirtualKey Decimal;

		static const VirtualKey A;
		static const VirtualKey B;
		static const VirtualKey C;
		static const VirtualKey D;
		static const VirtualKey E;
		static const VirtualKey F;
		static const VirtualKey G;
		static const VirtualKey H;
		static const VirtualKey I;
		static const VirtualKey J;
		static const VirtualKey K;
		static const VirtualKey L;
		static const VirtualKey M;
		static const VirtualKey N;
		static const VirtualKey O;
		static const VirtualKey P;
		static const VirtualKey Q;
		static const VirtualKey R;
		static const VirtualKey S;
		static const VirtualKey T;
		static const VirtualKey U;
		static const VirtualKey V;
		static const VirtualKey W;
		static const VirtualKey X;
		static const VirtualKey Y;
		static const VirtualKey Z;

		static const VirtualKey F1;
		static const VirtualKey F2;
		static const VirtualKey F3;
		static const VirtualKey F4;
		static const VirtualKey F5;
		static const VirtualKey F6;
		static const VirtualKey F7;
		static const VirtualKey F8;
		static const VirtualKey F9;
		static const VirtualKey F10;
		static const VirtualKey F11;
		static const VirtualKey F12;

		static const VirtualKey Tab;
		static const VirtualKey Enter;
		static const VirtualKey SpaceBar;
		static const VirtualKey BackSpace;
		static const VirtualKey Escape;

		static const VirtualKey Left;
		static const VirtualKey Right;
		static const VirtualKey Up;
		static const VirtualKey Down;

		static const VirtualKey Insert;
		static const VirtualKey Delete;
		static const VirtualKey PageUp;
		static const VirtualKey PageDown;
		static const VirtualKey Home;
		static const VirtualKey End;

		static const VirtualKey CapsLock;
		static const VirtualKey ScrollLock;
		static const VirtualKey NumLock;
		static const VirtualKey PrintScreen;
		static const VirtualKey Pause;

		static const VirtualKey LeftShift;
		static const VirtualKey RightShift;
		static const VirtualKey LeftControl;
		static const VirtualKey RightControl;
		static const VirtualKey LeftAlt;
		static const VirtualKey RightAlt;
		static const VirtualKey LeftCommand;
		static const VirtualKey RightCommand;

		static const VirtualKey Semicolon;
		static const VirtualKey Apostrophe;
		static const VirtualKey Comma;
		static const VirtualKey Period;
		static const VirtualKey Hyphen;
		static const VirtualKey Slash;
		static const VirtualKey BackSlash;
		static const VirtualKey Equals;
		static const VirtualKey LeftBracket;
		static const VirtualKey RightBracket;
		static const VirtualKey GraveAccent;

		static const VirtualKey Colon;
		static const VirtualKey Quote;
		static const VirtualKey Underscore;
		static const VirtualKey LeftParantheses;
		static const VirtualKey RightParantheses;
		static const VirtualKey Ampersand;
		static const VirtualKey Asterix;
		static const VirtualKey Caret;
		static const VirtualKey Dollar;
		static const VirtualKey Exclamation;

	public:
		static void Initialize();
		static void AddKey(KeyDetails& p_keydetails);

		static const Ref<KeyDetails> GetKeyDetails(const VirtualKey& p_key);
	private:
		static bool _initialized;

		static std::vector<Ref<KeyDetails>> _inputkeys;
	};
}