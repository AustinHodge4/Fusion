#pragma once

#include "virtual_key.h"

namespace Fusion {

	class FUSION_API KeyDetails
	{
	public:
		enum KeyFlags
		{
			GamepadKey = 1 << 0,
			MouseButton = 1 << 1,
			ModifierKey = 1 << 2,
			Axis1D = 1 << 3,
			Axis2D = 1 << 4,
			ButtonAxis = 1 << 5,

			NoFlags = 0,
		};

		KeyDetails(const VirtualKey& p_key, const std::string& p_displayName, const uint32_t p_flags = 0);

	public:
		VirtualKey& GetKey() { return _key; }
		const std::string& GetDisplayName() const { return _displayName; }

		inline const bool IsGamepadKey() const { return _isGamepad; }
		inline const bool IsModifierKey() const { return _isModifier; }
		inline const bool IsMouseButton() const { return _isMouseButton; }
	private:
		void Initialize(const uint32_t p_flags = 0);

		VirtualKey _key;
		std::string _displayName;
		
		bool _isGamepad;
		bool _isModifier;
		bool _isMouseButton;
	};
}