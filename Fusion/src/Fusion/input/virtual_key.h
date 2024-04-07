#pragma once

namespace Fusion {
	class KeyDetails;

	class FUSION_API VirtualKey 
	{
	public:
		friend class Key;

		VirtualKey(const std::string& p_name) : _name(p_name) {}

		bool IsValid() const;
		bool IsGamepadKey() const;
		bool IsModifierKey() const;
		bool IsMouseButton() const;

		inline const std::string& GetName() const { return _name; }

		inline bool operator==(const VirtualKey& p_key) const { return this->_name == p_key._name; }
		inline bool operator!=(const VirtualKey& p_key) { return this->_name != p_key._name; }
		inline bool operator<(const VirtualKey& p_key) const { return this->_name.compare(p_key._name) < 0; }

	private:
		std::string _name;

		mutable std::shared_ptr<KeyDetails> _keydetails;
	};
}