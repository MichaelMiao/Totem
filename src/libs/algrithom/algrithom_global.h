#ifndef ALGRITHOM_GLOBAL_H
#define ALGRITHOM_GLOBAL_H

#include <QtCore/qglobal.h>

#ifdef ALGRITHOM_LIB
# define ALGRITHOM_EXPORT Q_DECL_EXPORT
#else
# define ALGRITHOM_EXPORT Q_DECL_IMPORT
#endif

#endif // ALGRITHOM_GLOBAL_H
