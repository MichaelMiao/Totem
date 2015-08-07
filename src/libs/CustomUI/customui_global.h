#ifndef CUSTOMUI_GLOBAL_H
#define CUSTOMUI_GLOBAL_H

#include <qglobal.h>

#if defined(CUSTOMUI_LIB)
#  define CUSTOMUI_EXPORT Q_DECL_EXPORT
#elif  defined(CUSTOMUI_STATIC_LIB) // Abuse single files for manual tests
#  define CUSTOMUI_EXPORT
#else
#  define CUSTOMUI_EXPORT Q_DECL_IMPORT
#endif

#endif // CUSTOMUI_GLOBAL_H