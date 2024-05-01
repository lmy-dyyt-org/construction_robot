#pragma once

#include <exception>
#include <stdexcept>
#include <string>


//namespace std {

//	template <typename T>
//	class optional
//	{
//		bool _initialized;
//		std::aligned_storage_t<sizeof(T), alignof(T)> _storage;
//	public: 
//	// operations 
//	optional<std::string> tItem::findShortName()
//{
//	if (hasShortName)
//	{
//		return mShortName;
//	}
//	return std::nullopt;
//}
//	};
//	

//	
//	
//}

namespace ruckig {

//! The base class for all exceptions
struct RuckigError: public std::runtime_error {
    explicit RuckigError(const std::string& message): std::runtime_error("\n[ruckig] " + message) { }
};

}
