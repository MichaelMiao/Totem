#ifndef XMLDESERIALIZER_H
#define XMLDESERIALIZER_H
#include "../utils_global.h"
#include "xmlserializablefactory.h"
#include "../utilsconstants.h"

#include <QString>
#include <QtXML/QDomDocument>
#include <QtXML/QDomElement>
#include <QVector>
#include <QList>
namespace Utils{
class XmlSerializable;
class XmlSerializer;
class TOTEM_UTILS_EXPORT XmlDeserializer
{
public:
	XmlDeserializer(const QString &filename = QLatin1String(""));
	virtual ~XmlDeserializer(void);

	void deserialize(const QString &key, int &data);
	void deserialize(const QString &key, bool &data);
	void deserialize(const QString &key, double &data);
	void deserialize(const QString &key, float &data);
	void deserialize(const QString &key, QString &data);

	void deserialize(const QString &key, XmlSerializable &data);
	template<class T>
	void deserializeCollection(const QString &key, QList<T>&data, const QString &itemKey);
	template<class T>
	void deserializeCollection(const QString &key, QVector<T*>&data, const QString &itemKey);
	template<class T>
	void deserializeCollection(const QString &key, QList<T*>&data, const QString &itemKey);


protected:
	QString			m_filename;
	QDomElement		m_root;				//!< 根节点
	QDomDocument	m_doc;				//!<	file对应的doc变量
	QDomElement		m_currentElement;	//!< 当前操作的节点
};

template<class T>
void XmlDeserializer::deserializeCollection(const QString &key, QList<T>&data, const QString &itemKey)
{
	QDomElement tempElement = m_currentElement;
	m_currentElement = m_currentElement.firstChildElement(key);
	if (m_currentElement.isNull())
	{
		return;
	}
	QDomElement childElement = m_currentElement.firstChildElement(itemKey);
	while(!childElement.isNull())
	{
		m_currentElement = childElement;
		T tempData;
		tempData.deserialize(*this);
//		deserialize(itemKey, tempData);
		data.append(tempData);
		childElement = childElement.nextSiblingElement(itemKey);
	}
	m_currentElement = tempElement;
}
template<class T>
void XmlDeserializer::deserializeCollection( const QString &key, QVector<T*>&data, const QString &itemKey )
{
	QDomElement tempElement = m_currentElement;
	m_currentElement = m_currentElement.firstChildElement(key);
	if (m_currentElement.isNull())
	{
		return;
	}
	QDomElement childElement = m_currentElement.firstChildElement(itemKey);
	while(!childElement.isNull())
	{
		QString type = childElement.attribute(Constants::XML_NODE_TYPE);
		if (type.isEmpty())
		{
			continue;
		}
		m_currentElement = childElement;
		T *tempData = (T*)XmlSerializableFactory::instance()->createSerialzable(type);
		deserialize(itemKey, *tempData);
		data.append(tempData);
		childElement = childElement.nextSiblingElement(itemKey);
	}
	m_currentElement = tempElement;
}
template<class T>
void XmlDeserializer::deserializeCollection( const QString &key, QList<T*>&data, const QString &itemKey )
{
	QDomElement tempElement = m_currentElement;
	m_currentElement = m_currentElement.firstChildElement(key);
	if (m_currentElement.isNull())
	{
		return;
	}
	QDomElement childElement = m_currentElement.firstChildElement(itemKey);
	while(!childElement.isNull())
	{
		QString type = childElement.attribute(Constants::XML_NODE_TYPE);
		if (type.isEmpty())
		{
			continue;
		}
		m_currentElement = childElement;
		T *tempData = (T*)XmlSerializableFactory::instance()->createSerialzable(type);
		tempData->deserialize(*this);
		data.append(tempData);
		childElement = childElement.nextSiblingElement(itemKey);
	}
	m_currentElement = tempElement;
}


}
#endif
