#include "stdafx.h"
#include "boolproperty.h"
#include "../designnet_core_def.h"
#include "Utils/XML/xmldeserializer.h"
#include "Utils/XML/xmlserializable.h"
#include "Utils/XML/xmlserializer.h"


namespace DesignNet{

BoolProperty::BoolProperty( const QString &id, const QString &name, QObject *parent /*= 0*/ )
	: Property(id, name, parent)
{
	m_value = false;
}

BoolProperty::~BoolProperty()
{

}

void BoolProperty::setValue( const bool &value )
{
	if(value != m_value)
	{
		m_value = value;
		emit changed();
	}	
}

bool BoolProperty::value() const
{
	return m_value;
}

void BoolProperty::onValueChanged( bool bValue )
{
	setValue(bValue);
}

}
