#ifndef XMLSERIALIZER_H
#define XMLSERIALIZER_H

#include "../utils_global.h"
#include <QString>
#include <QtXml/QDomDocument>
#include <QtXml/QDomNode>
namespace Utils{
class XmlSerializable;
class TOTEM_UTILS_EXPORT XmlSerializer
{
public:
	XmlSerializer(const QString &filepath = QLatin1String(""), const QString &type = QLatin1String(""));
	virtual ~XmlSerializer();

	void setFilePath(const QString &filepath = QLatin1String(""));
	QString filePath() const;
	void serialize(const QString &key, const int &data);
	void serialize(const QString &key, const bool &data);
	void serialize(const QString &key, const float &data);
	void serialize(const QString &key, const double &data);
	void serialize(const QString &key, const QString &data);
	void serialize(const QString &key, const XmlSerializable &data);
	
	template<class T>
	void serialize(const QString &key, const QList<T> &datas, const QString &items) ;

	template<class T>
	void serialize(const QString &key, const QVector<T*> &datas, const QString &items) ;
	template<class T>
	void serialize(const QString &key, const QList<T*> &datas, const QString &items) ;

	void write(const QString &filePath);
protected:
	QString			m_filepath;			//!< 文件路径
	mutable QDomElement		m_root;				//!< 根节点
	mutable QDomDocument	m_doc;				//!<	file对应的doc变量
	mutable QDomElement		m_currentElement;	//!< 当前操作的节点
};
template<class T>
void Utils::XmlSerializer::serialize(const QString &key, const QList<T> &datas, const QString &itemKey) 
{
	QDomElement parentItem = m_doc.createElement(key);
	m_currentElement.appendChild(parentItem);
	QDomElement tempElement = m_currentElement;
	m_currentElement = parentItem;
	foreach(const T &data, datas)
	{
		serialize(itemKey, data);
	}
	m_currentElement = tempElement;
}

template<class T>
void Utils::XmlSerializer::serialize( const QString &key, const QVector<T*> &datas, const QString &itemKey )
{
	QDomElement parentItem = m_doc.createElement(key);
	m_currentElement.appendChild(parentItem);
	QDomElement tempElement = m_currentElement;
	m_currentElement = parentItem;
	foreach(T* data, datas)
	{
		serialize(itemKey, *data);
	}
	m_currentElement = tempElement;
}

template<class T>
void Utils::XmlSerializer::serialize( const QString &key, const QList<T*> &datas, const QString &itemKey )
{
	QDomElement parentItem = m_doc.createElement(key);
	m_currentElement.appendChild(parentItem);
	QDomElement tempElement = m_currentElement;
	m_currentElement = parentItem;
	foreach(T* data, datas)
	{
		serialize(itemKey, *data);
	}
	m_currentElement = tempElement;
}
}

#endif // XMLSERIALIZER_H
