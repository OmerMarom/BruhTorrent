#pragma once

#if defined(BT_PLATFORM_WINDOWS) && defined(BT_SHARED)
	#ifdef BT_EXPORT
		#define BT_API __declspec(dllexport)
	#else
		#define BT_API __declspec(dllimport)
	#endif
#else
	#define BT_API
#endif

#ifdef BT_PLATFORM_WINDOWS
	#define BT_ABS_CLASS abstract
#else
	#define BT_ABS_CLASS
#endif
