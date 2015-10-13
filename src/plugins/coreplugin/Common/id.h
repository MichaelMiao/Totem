#pragma once
#include <QMetaType>
#include "../core_global.h"


namespace Core
{

class CORE_EXPORT Id
{
public:

    Id() : m_id(0) {}
    Id(const char *name);

    explicit Id(const QString &name);
    QByteArray name() const;
    QString toString() const;

    bool isValid() const { return m_id; }
    bool operator==(const Id &id) const { return m_id == id.m_id; }
    bool operator!=(const Id &id) const { return m_id != id.m_id; }

    int uniqueIdentifier() const { return m_id; }
    static Id fromUniqueIdentifier(int uid) { return Id(uid); }

private:

    Id(int uid) : m_id(uid) {}
    Id(const QLatin1String &);
    
	int m_id;
};

inline uint qHash(const Id &id) { return id.uniqueIdentifier(); }

}
Q_DECLARE_METATYPE(Core::Id)
