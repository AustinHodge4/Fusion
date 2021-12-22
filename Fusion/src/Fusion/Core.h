#pragma once

#ifdef FE_PLATFORM_WINDOWS
	#ifdef FE_BUILD_DLL
		#define FUSION_API __declspec(dllexport)
	#else
		#define FUSION_API __declspec(dllimport)
	#endif
#else
	#error Fusion only supports Windows!
#endif