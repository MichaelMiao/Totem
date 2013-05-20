#include "xmldeserializer.h"
#include "xmlserializable.h"
#include "xmlserializer.h"
#include <QFile>
namespace Utils{

XmlDeserializer::XmlDeserializer( const QString &filename /*= QLatin1String("")*/ )
	: m_filename(filename)
{
	QFile file(m_filename);
	if (!file.open(QIODevice::ReadOnly))
	{
		return ;
	}
	if(true == m_doc.setContent(&file))
	{
		m_currentElement = m_root = m_doc.documentElement();
	}
	file.close();

}

XmlDeserializer::~XmlDeserializer(void)
{
}

void XmlDeserializer::deserialize(const QString &key, XmlSerializable &data) 
{
	QDomElement tempElement = m_currentElement;
	m_currentElement = m_currentElement.firstChildElement(key);
	if (m_currentElement.isNull())
	{
		m_currentElement = tempElement;
		return;
	}
	data.deserialize(*this);
	m_currentElement = tempElement;
}

void XmlDeserializer::deserialize( const QString &key, int &data )
{
	if (m_currentElement.hasAttribute(key))
	{
		data = m_currentElement.attribute(key).toInt();
	}
}

void XmlDeserializer::deserialize( const QString &key, bool &data )
{
	if (m_currentElement.hasAttribute(key))
	{
		if( m_currentElement.attribute(key) == Constants::XML_VALUE_TRUE )
		{
			data = true;
		}
		else
		{
			data = false;
		}
	}
}


void XmlDeserializer::deserialize( const QString &key, double &data )
{
	if (m_currentElement.hasAttribute(key))
	{
		data = m_currentElement.attribute(key).toDouble();
	}
}


void XmlDeserializer::deserialize( const QString &key, float &data )
{
	if (m_currentElement.hasAttribute(key))
	{
		data = m_currentElement.attribute(key).toFloat();
	}
}

void XmlDeserializer::deserialize( const QString &key, QString &data )
{
	if (m_currentElement.hasAttribute(key))
	{
		data = m_currentElement.attribute(key);
	}
}




}