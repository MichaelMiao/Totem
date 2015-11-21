#include "stdafx.h"
#include "property.h"
#include "propertyowner.h"


DesignNet::Property::Property(const QString &id, const QString &name, QObject *parent) :
	QObject(parent),
	m_id(id),
	m_name(name)
{
}

QString DesignNet::Property::name() const
{
	return m_name;
}

void DesignNet::Property::setName(const QString &name)
{
	m_name = name;
}

QString DesignNet::Property::id() const
{
	return m_id;
}

void DesignNet::Property::setId(const QString &id)
{
	m_id = id;
}


DesignNet::PropertyOwner *DesignNet::Property::owner() const
{
	return m_owner;
}

void DesignNet::Property::setOwner(DesignNet::PropertyOwner *owner)
{
	m_owner = owner;
}

QString DesignNet::Property::serializableType() const
{
	return m_id;
}

void DesignNet::Property::serialize(Utils::XmlSerializer& s) const
{

}

void DesignNet::Property::deserialize(Utils::XmlDeserializer& s)
{

}

Utils::XmlSerializable* DesignNet::Property::createSerializable() const
{
	return NULL;
}
