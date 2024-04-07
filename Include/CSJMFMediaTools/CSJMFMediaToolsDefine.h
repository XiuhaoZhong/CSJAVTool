#pragma once

#ifdef CSJMFMEDIATOOLS_EXPORTS
#define CSJMFMEDIATOOLS_API __declspec(dllexport)
#else
#define CSJMFMEDIATOOLS_API __declspec(dllimport)
#endif

#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
