#include "xmlserializablefactory.h"
#include "xmlserializable.h"
#include <QHash>
namespace Utils{

class XmlSerializableFactoryPrivate
{
public:
	XmlSerializableFactoryPrivate(){}
	QHash<QString, XmlSerializable*> m_serializables;//!< 所有可序列类型
};

XmlSerializableFactory* XmlSerializableFactory::m_instance = 0;
XmlSerializableFactory::XmlSerializableFactory(QObject *parent)
	: QObject(parent),
	d(new XmlSerializableFactoryPrivate())
{
	m_instance = this;
}

XmlSerializableFactory::~XmlSerializableFactory()
{

}

XmlSerializableFactory * XmlSerializableFactory::instance()
{
	if (!m_instance)
	{
		m_instance = new XmlSerializableFactory();
	}
	return m_instance;
}

void XmlSerializableFactory::Release()
{
	if (m_instance)
	{
		delete m_instance;
		m_instance = 0;
	}
}

void XmlSerializableFactory::registerSerializable( const XmlSerializable* serializable )
{
	QString serializableType = serializable->serializableType();
	if (d->m_serializables.contains(serializableType))
	{
		return;
	}
	XmlSerializable* s = serializable->createSerializable();
	d->m_serializables.insert(serializableType, s);
}

XmlSerializable* XmlSerializableFactory::createSerialzable( const QString &serialzableType ) 
{
	XmlSerializable* ret = d->m_serializables.value(serialzableType);
	return ret->createSerializable();
}

}
