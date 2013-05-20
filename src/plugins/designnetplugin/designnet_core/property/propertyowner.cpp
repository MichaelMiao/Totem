#include "propertyowner.h"
#include "coreplugin/icore.h"
#include "property.h"
#include "Utils/XML/xmlserializer.h"
#include "Utils/XML/xmlserializable.h"
#include <QDebug>
using namespace Aggregation;
namespace DesignNet{

PropertyOwner::PropertyOwner()
{
    m_aggregate = new Aggregate(false);
}

PropertyOwner::~PropertyOwner()
{
    if(m_aggregate)
    {
        delete m_aggregate;
    }
}

Property *PropertyOwner::getProperty(const QString &id) const
{
    foreach(Property *property, m_aggregate->components<Property>())
    {
        if(property->id() == id)
            return property;
    }
    return 0;
}

QList<Property *> PropertyOwner::getProperties() const
{
    return m_aggregate->components<Property>();
}

void PropertyOwner::setPropertyGroupLabel(const QString &id, const QString &label)
{
    m_propertyGroupIDLabelMap[id] = label;
}

QString PropertyOwner::propertyGroupLabel(const QString &id) const
{
    return m_propertyGroupIDLabelMap.value(id, "");
}

void PropertyOwner::addProperty(Property *prop)
{
    if(prop->id().isEmpty())
    {
        qWarning() << "The id of this property should not be empty.";
        return;
    }
    if(getProperty(prop->id()))
    {
        qWarning() << "The id of this property has been added";
        return ;
    }
    m_aggregate->add(prop);
    prop->setOwner(this);
	propertyAdded(prop);
}


void PropertyOwner::removeProperty(Property *prop)
{
    if(!getProperty(prop->id()))
    {
        qWarning() << "The id of this property doesn't exist";
        return ;
    }
    propertyRemoving(prop);
    prop->setOwner(0);
    m_aggregate->remove(prop);
    propertyRemoved(prop);
}

bool PropertyOwner::checkProperty( Property *prop /*= 0*/ )
{
	if(prop == 0)
	{
		QList<Property*> list = m_aggregate->components<Property>();
		foreach(Property *p, list)
		{
			if(!p->isValid())
				return false;
		}
		return true;
	}
	return prop->isValid();
}

QList<Property*> PropertyOwner::getInvalidProperties()
{
	QList<Property*> list;
	foreach(Property *p, m_aggregate->components<Property>())
	{
		if(!p->isValid())
		{
			list << p;
		}
	}
	return list;
}

void PropertyOwner::serialize( Utils::XmlSerializer& s )const
{
	foreach(Property *p, m_aggregate->components<Property>())
	{
		s.serialize("Property", *p);
	}
}

void PropertyOwner::deserialize( Utils::XmlDeserializer& s )
{

}

}
