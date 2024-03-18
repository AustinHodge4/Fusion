#include "fepch.h"
#include "key_details.h"

namespace Fusion {

	KeyDetails::KeyDetails(const VirtualKey& p_key, const std::string& p_displayName, const uint32_t p_flags)
		: _key(p_key), _displayName(p_displayName)
	{
		Initialize(p_flags);
	}

	void KeyDetails::Initialize(const uint32_t p_flags)
	{
		_isGamepad = (p_flags & KeyDetails::GamepadKey) != 0;
		_isModifier = (p_flags & KeyDetails::ModifierKey) != 0;
		_isMouseButton = (p_flags & KeyDetails::MouseButton) != 0;
	}
}
