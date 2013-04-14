#include "id.h"
#include "utils/totemassert.h"
#include <QHash>

namespace Core{
/*!
    \class Core::Id
    \brief 将字符串压缩成一个int型的身份变量m_id
*/
class StringHolder
{
public:
    explicit StringHolder(const char *s)
        : str(s)
    {
        n = strlen(s);
        int m = n;
        h = 0;
        while (m--)
        {
            h = (h << 4) + *s++;
            h ^= (h & 0xf0000000) >> 23;
            h &= 0x0fffffff;
        }
    }
    int n;
    const char *str;
    uint h;
};

static bool operator==(const StringHolder &sh1, const StringHolder &sh2)
{
    return sh1.h == sh2.h && strcmp(sh1.str, sh1.str) == 0;
}

static uint qHash(const StringHolder &sh)
{
    return sh.h;
}

struct IdCache : public QHash<StringHolder, int>
{
#ifndef QTC_ALLOW_STATIC_LEAKS
    ~IdCache()
    {
        for (IdCache::iterator it = begin(); it != end(); ++it)
            free(const_cast<char *>(it.key().str));
    }
#endif
};
static int lastUid = 0;
static QVector<QByteArray> stringFromId;
static IdCache idFromString;

static int theId(const char *str)
{
    TOTEM_ASSERT(str && *str, return 0);
    StringHolder sh(str);
    int res = idFromString.value(sh, 0);
    if (res == 0)
    {
        if (lastUid == 0)
            stringFromId.append(QByteArray());
        res = ++lastUid;

        sh.str = strdup(sh.str);//这里由调用者负责管理内存
        idFromString[sh] = res;
        stringFromId.append(QByteArray::fromRawData(sh.str, sh.n));
    }
    return res;
}

Id::Id(const char *name)
    : m_id(theId(name))
{}

Id::Id(const QString &name)
   : m_id(theId(name.toLatin1()))
{}

QByteArray Id::name() const
{
    return stringFromId.at(m_id);
}

QString Id::toString() const
{
    return QString::fromLatin1(stringFromId[m_id]);
}

}
