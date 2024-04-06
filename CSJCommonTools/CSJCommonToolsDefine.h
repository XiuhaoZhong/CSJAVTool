#pragma once
#ifdef CSJCOMMONTOOLS_EXPORTS
#define CSJCOMMONTOOLS_API __declspec(dllexport)
#else
#define CSJCOMMONTOOLS_API __declspec(dllimport)
#endif
