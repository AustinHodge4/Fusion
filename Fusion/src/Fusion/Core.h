#pragma once

#ifdef FE_PLATFORM_WINDOWS
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