#ifndef XMLSERIALIZABLEFACTORY_H
#define XMLSERIALIZABLEFACTORY_H
#include "../utils_global.h"
#include <QObject>
namespace Utils{

/**
 * \class	XmlSerializableFactory
 *
 * \brief	XML 序列化工厂，用于生成各种各样的Serializable对象
 * 			
 *	XML工厂是一个单例模式，\note 使用该工厂的最后一定要调用Release()
 *
 * \author	Michael_BJFU
 * \date	2013/5/15
 */

class XmlSerializableFactoryPrivate;
class XmlSerializable;

class TOTEM_UTILS_EXPORT XmlSerializableFactory : public QObject
{
	Q_OBJECT

public:
	XmlSerializableFactory(QObject *parent = 0);
	~XmlSerializableFactory();
	static XmlSerializableFactory *instance();
	static void Release();
	void registerSerializable(const XmlSerializable* serializable);
	XmlSerializable* createSerialzable(const QString &serialzableType) ;
private:
	XmlSerializableFactoryPrivate *d;
	static XmlSerializableFactory *m_instance;
};

}

#endif // XMLSERIALIZABLEFACTORY_H
