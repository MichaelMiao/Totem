#ifndef UTILS_GLOBAL_H
#define UTILS_GLOBAL_H

#include <qglobal.h>

#if defined(TOTEM_UTILS_LIB)
#  define TOTEM_UTILS_EXPORT Q_DECL_EXPORT
#else
#  define TOTEM_UTILS_EXPORT Q_DECL_IMPORT
#endif

#endif // UTILS_GLOBAL_H