#ifndef CUSTOMUI_GLOBAL_H
#define CUSTOMUI_GLOBAL_H

#include <qglobal.h>

#if defined(CUSTOMUI_LIB)
#  define CUSTOMUI_EXPORT __declspec(dllexport)
#elif  defined(CUSTOMUI_STATIC_LIB) // Abuse single files for manual tests
#error 1
#  define CUSTOMUI_EXPORT
#else
#  define CUSTOMUI_EXPORT __declspec(dllimport)
#endif

#endif // CUSTOMUI_GLOBAL_H