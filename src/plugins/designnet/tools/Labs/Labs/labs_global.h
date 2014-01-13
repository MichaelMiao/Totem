#ifndef LABS_GLOBAL_H
#define LABS_GLOBAL_H

#include <QtCore/qglobal.h>

#ifdef LABS_LIB
# define LABS_EXPORT Q_DECL_EXPORT
#else
# define LABS_EXPORT Q_DECL_IMPORT
#endif

#endif // LABS_GLOBAL_H
