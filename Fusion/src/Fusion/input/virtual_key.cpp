#include "fepch.h"
#include "virtual_key.h"

#include "input_keys.h"

namespace Fusion {

    bool VirtualKey::IsValid() const
    {
        if (_name != "Invalid") {
            if (_keydetails == nullptr)
            {
                _keydetails = Key::GetKeyDetails(*this);
            }
            return _keydetails != nullptr;
        }
        return false;
    }

    bool VirtualKey::IsGamepadKey() const
    {
        return IsValid() ? _keydetails->IsGamepadKey() : false;
    }

    bool VirtualKey::IsModifierKey() const
    {
        return IsValid() ? _keydetails->IsModifierKey() : false;
    }

    bool VirtualKey::IsMouseButton() const
    {
        return IsValid() ? _keydetails->IsMouseButton() : false;
    }
}
