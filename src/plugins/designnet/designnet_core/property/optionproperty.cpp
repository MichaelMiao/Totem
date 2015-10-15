#include "optionproperty.h"
#include "designnetconstants.h"
#include "Utils/XML/xmlserializer.h"
#include "Utils/XML/xmlserializable.h"
#include "Utils/XML/xmldeserializer.h"

#include <QDebug>
namespace DesignNet{

OptionProperty::OptionProperty( const QString &id, const QString &name, QObject *parent /*= 0*/ )
	: Property(id, name, parent)
{

}

OptionProperty::~OptionProperty()
{

}

void OptionProperty::addOption( const QString&text, QVariant data /*= QVariant()*/ )
{
	m_options[text] = data;
	select(text);
}

Core::Id OptionProperty::propertyType() const
{
	return DesignNet::Constants::PROPERTY_TYPE_OPTION;
}

bool OptionProperty::isValid() const
{
	if(m_currentKey.isNull())
		return true;
	return false;
}

void OptionProperty::select( const QString &text )
{
	if(m_options.contains(text))
	{
		m_currentKey = text;
		emit changed();
	}
}

QList<QString> OptionProperty::keys()
{
	return m_options.keys();
}

QVariant OptionProperty::getValue( const QString &key )
{
	return m_options.value(key, QVariant());
}

void OptionProperty::serialize( Utils::XmlSerializer& s )const
{
	Property::serialize(s);
	s.serialize("value", m_currentKey);
}

void OptionProperty::deserialize( Utils::XmlDeserializer& s )
{

}

}
