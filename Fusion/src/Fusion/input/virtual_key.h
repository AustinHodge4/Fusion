#pragma once

#include "fusion/core.h"

namespace fusion {
	class KeyDetails;

	class FUSION_API VirtualKey 
	{
	public:
		friend class Key;

		VirtualKey(std::string name) : _name(name) {}

		bool is_valid() const;
		bool is_gamepad_key() const;
		bool is_modifier_key() const;
		bool is_mouse_button() const;

		inline const std::string get_name() const { return _name; }

		inline bool operator==(const VirtualKey& x) const { return this->_name == x._name; }
		inline bool operator!=(const VirtualKey& x) { return this->_name != x._name; }
		inline bool operator<(const VirtualKey& x) const { return this->_name.compare(x._name) < 0; }

	private:
		std::string _name;

		mutable std::shared_ptr<KeyDetails> _key_details;
	};
}