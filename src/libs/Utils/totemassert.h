#ifndef TOTEMASSERT_H
#define TOTEMASSERT_H


#include <QDebug>

#define TOTEM_ASSERT_STRINGIFY_INTERNAL(x) #x
#define TOTEM_ASSERT_STRINGIFY(x) TOTEM_ASSERT_STRINGIFY_INTERNAL(x)

#define TOTEM_ASSERT(cond, action) \
    if(cond){}else{qDebug()<<"SOFT ASSERT: \""#cond"\" in file " __FILE__ ", line " TOTEM_ASSERT_STRINGIFY(__LINE__);action;}

#define TOTEM_CHECK(cond) \
    if(cond){}else{qDebug()<<"SOFT ASSERT: \""#cond"\" in file " __FILE__ ", line " TOTEM_ASSERT_STRINGIFY(__LINE__);}

#endif // TOTEMASSERT_H