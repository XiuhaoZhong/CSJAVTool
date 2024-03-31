#pragma once

#ifdef CSJMEDIACAPTURE_EXPORTS
#define CSJMEDIACAPTURE_API __declspec(dllexport)
#else
#define CSJMEDIACAPTURE_API __declspec(dllimport)
#endif
