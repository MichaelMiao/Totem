#ifndef CORE_GLOBAL_H
#define CORE_GLOBAL_H

#include <QtCore/qglobal.h>

#ifdef CORE_LIB
# define CORE_EXPORT Q_DECL_EXPORT
#else
# define CORE_EXPORT Q_DECL_IMPORT
#endif

#endif // CORE_GLOBAL_H
