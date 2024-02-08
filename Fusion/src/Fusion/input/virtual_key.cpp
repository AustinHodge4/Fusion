#include "fepch.h"
#include "virtual_key.h"

#include "input_keys.h"

namespace fusion {

    bool VirtualKey::is_valid() const
    {
        if (_name != "Invalid") {
            if (_key_details == nullptr)
            {
                _key_details = Key::get_key_details(*this);
            }
            return _key_details != nullptr;
        }
        return false;
    }

    bool VirtualKey::is_gamepad_key() const
    {
        return is_valid() ? _key_details->is_gamepad_key() : false;
    }

    bool VirtualKey::is_modifier_key() const
    {
        return is_valid() ? _key_details->is_modifier_key() : false;
    }

    bool VirtualKey::is_mouse_button() const
    {
        return is_valid() ? _key_details->is_mouse_button() : false;
    }
}
