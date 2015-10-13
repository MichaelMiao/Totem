#pragma once

#include <QtCore/qglobal.h>

#ifdef DESIGNNET_CORE_LIB
# define DESIGNNET_CORE_EXPORT Q_DECL_EXPORT
#else
# define DESIGNNET_CORE_EXPORT Q_DECL_IMPORT
#endif