#pragma once

#ifdef CSJMEDIAPEGTOOLS_EXPORTS
#define CSJMEDIAPEGTOOL_API __declspec(dllexport)
#else
#define CSJMEDIAPEGTOOL_API __declspec(dllimport)
#endif
