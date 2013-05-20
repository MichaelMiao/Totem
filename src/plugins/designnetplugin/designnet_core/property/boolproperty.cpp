#include "boolproperty.h"
#include "designnetconstants.h"
#include "Utils/XML/xmlserializer.h"
#include "Utils/XML/xmlserializable.h"
#include "Utils/XML/xmldeserializer.h"

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

Core::Id BoolProperty::typeID() const
{
	return DesignNet::Constants::PROPERTY_TYPE_BOOL;
}

bool BoolProperty::isValid() const
{
	return true;
}

void BoolProperty::serialize( Utils::XmlSerializer& s ) const
{
	Property::serialize(s);
	s.serialize("value", m_value);
}

void BoolProperty::deserialize( Utils::XmlDeserializer& s ) 
{

}

}
