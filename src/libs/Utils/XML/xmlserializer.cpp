#include "xmlserializer.h"
#include "utilsconstants.h"
#include "xmlserializable.h"
#include <QFile>
namespace Utils{


XmlSerializer::XmlSerializer( const QString &filepath /*= QLatin1String("")*/, const QString &type )
	: m_doc(type)
{
	m_filepath = filepath;
	m_root = m_doc.createElement(Constants::XML_ROOT);
	m_root.setAttribute(Constants::XML_VERSION_ATRRIBUTE, QLatin1String(Constants::XML_VERSION));
	m_currentElement = m_root;
	m_doc.appendChild(m_root);
}

XmlSerializer::~XmlSerializer()
{
}

QString XmlSerializer::filePath() const
{
	return m_filepath;
}

void XmlSerializer::setFilePath( const QString &filepath /*= QLatin1String("")*/ )
{
	m_filepath = filepath;
}

void XmlSerializer::createElement(const QString &key)
{
	QDomElement parentItem = m_doc.createElement(key);
	m_currentElement.appendChild(parentItem);
}

void XmlSerializer::serialize( const QString &key, const XmlSerializable& data )
{
	QDomElement elem = m_doc.createElement(key);
	QDomElement oldElem = m_currentElement;
	m_currentElement = elem;
	data.serialize(*this);
	if (!data.serializableType().isEmpty())
	{
		this->serialize(Constants::XML_NODE_TYPE, data.serializableType());
	}
	m_currentElement = oldElem;
	m_currentElement.appendChild(elem);
}

void XmlSerializer::serialize( const QString &key, const int &data )
{
	m_currentElement.setAttribute(key, data);
}

void XmlSerializer::serialize( const QString &key, const bool &data )
{
	m_currentElement.setAttribute(key, data ? Constants::XML_VALUE_TRUE : Constants::XML_VALUE_FALSE);
}

void XmlSerializer::serialize( const QString &key, const float &data )
{
	m_currentElement.setAttribute(key, data);
}

void XmlSerializer::serialize( const QString &key, const double &data )
{
	m_currentElement.setAttribute(key, data);
}

void XmlSerializer::serialize( const QString &key, const QString &data )
{
	m_currentElement.setAttribute(key, data);
}

void XmlSerializer::write(const QString &filePath)
{
	QFile file(filePath);
	if (file.open(QIODevice::WriteOnly))
	{
		file.write(m_doc.toByteArray(4));
		file.close();
	}
}

}

