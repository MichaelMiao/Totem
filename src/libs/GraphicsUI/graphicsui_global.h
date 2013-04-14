#ifndef GRAPHICSUI_GLOBAL_H
#define GRAPHICSUI_GLOBAL_H

#include <qglobal.h>

#if defined(TOTEM_GRAPHICSUI_LIB)
#  define TOTEM_GRAPHICSUI_EXPORT Q_DECL_EXPORT
#elif  defined(TOTEM_GRAPHICSUI_STATIC_LIB) // Abuse single files for manual tests
#  define TOTEM_GRAPHICSUI_EXPORT
#else
#  define TOTEM_GRAPHICSUI_EXPORT Q_DECL_IMPORT
#endif

#endif // GRAPHICSUI_GLOBAL_H