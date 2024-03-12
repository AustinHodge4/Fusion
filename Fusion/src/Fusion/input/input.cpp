#include "fepch.h"
#include "input.h"

#include "input_keys.h"

namespace fusion {

	std::unordered_map<int, VirtualKey> Input::_key_map;

	const VirtualKey& Input::get_key_from_codes(const int keycode)
	{
		auto entry = _key_map.find(keycode);
		if (entry == _key_map.end()) {
			FE_CORE_WARN("Key code does not have a key!!: {0}", keycode);
			return Key::Invalid;
		}

		return entry->second;
	}

	int Input::get_codes_from_key(const VirtualKey& key)
	{
		auto entry = std::find_if(_key_map.begin(), _key_map.end(), [key](auto& pair) { return pair.second == key; });
		if (entry == _key_map.end()) {
			FE_CORE_ERROR("Key does not have a keycode!! {0}", key.get_name());
			return -1;
		}
		return entry->first;
	}

	void Input::initialize_keys()
	{
		unsigned int keycodes[256];
		std::string key_names[256];

		unsigned int count = PlatformInput::get_platform_keys(keycodes, key_names);
		FE_CORE_INFO("Platform keys mappings: {0}", count);

		_key_map.clear();

		for (unsigned int i = 0; i < count; i++)
		{
			VirtualKey key(key_names[i]);

			if (!key.is_valid()) {
				FE_CORE_ERROR("Platform key is not valid!");
				continue;
			}
			_key_map.insert({ keycodes[i], key });
		}
	}

}