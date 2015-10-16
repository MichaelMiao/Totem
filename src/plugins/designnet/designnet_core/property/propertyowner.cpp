#include "stdafx.h"
#include "propertyowner.h"
#include "coreplugin/icore.h"
#include "property.h"
#include <QDebug>


using namespace Aggregation;
namespace DesignNet
{
PropertyOwner::PropertyOwner()
{
	m_aggregate = new Aggregate(false);
}

PropertyOwner::~PropertyOwner()
{
	if (m_aggregate)
	{
		delete m_aggregate;
	}
}

Property* PropertyOwner::getProperty(const QString& id) const
{
	foreach (Property* property, m_aggregate->components<Property>())
	{
		if (property->id() == id)
			return property;
	}

	return 0;
}

QList<Property*> PropertyOwner::getProperties() const
{
	return m_aggregate->components<Property>();
}

void PropertyOwner::setPropertyGroupLabel(const QString& id, const QString& label)
{
	m_propertyGroupIDLabelMap[id] = label;
}

QString PropertyOwner::propertyGroupLabel(const QString& id) const
{
	return m_propertyGroupIDLabelMap.value(id, "");
}

void PropertyOwner::addProperty(Property* prop)
{
	if (prop->id().isEmpty())
	{
		qWarning() << "The id of this property should not be empty.";
		return;
	}

	if (getProperty(prop->id()))
	{
		qWarning() << "The id of this property has been added";
		return ;
	}

	m_aggregate->add(prop);
	prop->setOwner(this);
	propertyAdded(prop);
}


void PropertyOwner::removeProperty(Property* prop)
{
	if (!getProperty(prop->id()))
	{
		qWarning() << "The id of this property doesn't exist";
		return ;
	}

	propertyRemoving(prop);
	prop->setOwner(0);
	m_aggregate->remove(prop);
	propertyRemoved(prop);
}

void PropertyOwner::propertyRemoving(Property* prop)
{

}

void PropertyOwner::propertyRemoved(Property* prop)
{

}

void PropertyOwner::propertyAdded(Property* prop)
{

}

}
