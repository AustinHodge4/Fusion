#include "fepch.h"
#include "input.h"

#include "input_keys.h"

namespace Fusion {

	std::unordered_map<int, VirtualKey> Input::_keymap;

	const VirtualKey& Input::GetKeyFromCodes(const int p_keycode)
	{
		auto entry = _keymap.find(p_keycode);
		if (entry == _keymap.end()) {
			FE_CORE_WARN("Key code does not have a key!!: {0}", p_keycode);
			return Key::Invalid;
		}

		return entry->second;
	}

	int Input::GetCodesFromKey(const VirtualKey& p_key)
	{
		auto entry = std::find_if(_keymap.begin(), _keymap.end(), [p_key](auto& pair) { return pair.second == p_key; });
		if (entry == _keymap.end()) {
			FE_CORE_ERROR("Key does not have a keycode!! {0}", p_key.GetName());
			return -1;
		}
		return entry->first;
	}

	void Input::InitializeKeys()
	{
		unsigned int keycodes[256];
		std::string keynames[256];

		unsigned int count = PlatformInput::GetPlatformKeys(keycodes, keynames);
		FE_CORE_INFO("Platform keys mappings: {0}", count);

		_keymap.clear();

		for (unsigned int i = 0; i < count; i++)
		{
			VirtualKey key(keynames[i]);

			if (!key.IsValid()) {
				FE_CORE_ERROR("Platform key is not valid!");
				continue;
			}
			_keymap.insert({ keycodes[i], key });
		}
	}

}