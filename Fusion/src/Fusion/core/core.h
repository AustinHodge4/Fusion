#pragma once

#include <memory>

#if defined(FE_PLATFORM_WINDOWS) || defined(FE_PLATFORM_LINUX)
#if FE_DYNAMIC_LINK
	#ifdef FE_BUILD_DLL
		#define FUSION_API __declspec(dllexport)
	#else
		#define FUSION_API __declspec(dllimport)
	#endif
#else
	#define FUSION_API 
#endif
#else
	#error Fusion only supports Windows!
#endif

#ifdef FE_DEBUG
	#define FE_ENABLE_ASSERTS
#endif

#ifdef FE_ENABLE_ASSERTS
	#define FE_ASSERT(x, ...) { if(!(x)) { FE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak();} }
	#define FE_CORE_ASSERT(x, ...) { if(!(x)) { FE_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak();} }
#else
	#define FE_ASSERT(x, ...)
	#define FE_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)

#define FE_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

namespace Fusion {
	template<typename T>
	using Scope = std::unique_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
}

#include "log.h"