#include "test.h"
#include <QtGui/QApplication>
#include "Utils/XML/xmlserializer.h"
#include "Utils/XML/xmlserializable.h"
#include "Utils/XML/xmldeserializer.h"
#include "Utils/XML/xmlserializablefactory.h"
using namespace Utils;
class A: public Utils::XmlSerializable
{
public:
	A(int i = 0){m_id = i;}
	virtual QString serializableType() const
	{  
		return "A";
	} 
	virtual A* createSerializable() const
	{	
		return new A; 
	}
	virtual void serialize(XmlSerializer& s) const
	{
		s.serialize("id", m_id);
	}
	virtual void deserialize(XmlDeserializer& s) 
	{
		s.deserialize("id", m_id);
	}
	int m_id;
};

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	Utils::XmlSerializer x;
	x.setFilePath("F:/miao.txt");
	A a;
	QList<A*> alist;
	for (int i = 0; i < 3; i++)
	{
		A *a = new A(i);
		alist << a;
	}
	x.serialize("AList", alist, "A");
	
	x.write("F:/miao.txt");
	alist.clear();
	XmlSerializableFactory *factory = XmlSerializableFactory::instance();
	factory->registerSerializable(new A(0));
	XmlDeserializer deserializer("F:/miao.txt");
	deserializer.deserializeCollection("AList", alist, "A");
	return 0;
}
