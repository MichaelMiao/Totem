#ifndef XMLSERIALIZABLE_H
#define XMLSERIALIZABLE_H

#include "../utils_global.h"
#define DECLARE_SERIALIZABLE(TNAME, T) \
	virtual QString serializableType() const\
	{  \
		return #TNAME; \
	} \
	virtual T* createSerializable() const\
	{	\
		return new T; \
	}
#define DECLARE_SERIALIZABLE_NOTYPE(T) \
	virtual QString serializableType() const\
	{  \
	return QString(); \
	} \
	virtual T* createSerializable() const\
	{	\
	return new T; \
	}
namespace Utils{
class XmlDeserializer;
class XmlSerializer;
class TOTEM_UTILS_EXPORT XmlSerializable
{
public:
	XmlSerializable(void);
	virtual ~XmlSerializable(void);
	virtual QString serializableType() const= 0;
	virtual void serialize(XmlSerializer& s) const = 0;
	virtual void deserialize(XmlDeserializer& s) = 0;
	virtual XmlSerializable* createSerializable() const = 0;
};

}

#endif // XMLSERIALIZER_H