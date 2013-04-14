#include "property.h"
#include "propertyowner.h"
namespace DesignNet{

Property::Property(const QString &id, const QString &name, QObject *parent) :
    QObject(parent),
    m_id(id),
    m_name(name)
{
}

QString Property::name() const
{
    return m_name;
}

void Property::setName(const QString &name)
{
    m_name = name;
}

QString Property::id() const
{
    return m_id;
}

void Property::setId(const QString &id)
{
    m_id = id;
}


PropertyOwner *Property::owner() const
{
    return m_owner;
}

void Property::setOwner(PropertyOwner *owner)
{
    m_owner = owner;
}
}
