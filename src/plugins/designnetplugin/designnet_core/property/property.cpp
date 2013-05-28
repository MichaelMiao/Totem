#include "property.h"
#include "propertyowner.h"
#include "Utils/XML/xmlserializer.h"
#include "Utils/XML/xmldeserializer.h"
#include "Utils/XML/xmlserializable.h"
using namespace Utils;
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

void Property::serialize( Utils::XmlSerializer& s ) const
{
	s.serialize("typeID", typeID().toString());
	s.serialize("ID", m_id);	
	s.serialize("Name", m_name);
}

void Property::deserialize( Utils::XmlDeserializer& s )
{

}

XmlSerializable* Property::createSerializable() const
{
	return 0;
}

QString Property::serializableType() const
{
	return typeID().toString();
}

Property::~Property()
{

}

}
