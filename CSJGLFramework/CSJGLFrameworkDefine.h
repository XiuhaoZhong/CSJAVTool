#ifndef __CSJGLFRAMEWORKDEFINE_H__
#define __CSJGLFRAMEWORKDEFINE_H__

#ifdef CSJGLFRAMEWORK_EXPORTS
#define CSJGLFRAMEWORK_API __declspec(dllexport)
#else
#define CSJGLFRAMEWORK_API __declspec(dllimport)
#endif

#endif // __CSJGLFRAMEWORKDEFINE_H__
