#pragma once

#ifdef CSJMEDIACAPTURE_EXPORTS
#define CSJMEDIACAPTURE_API __declspec(dllexport)
#else
#define CSJMEDIACAPTURE_API __declspec(dllimport)
#endif

#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
