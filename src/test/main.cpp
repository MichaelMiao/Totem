#include "test.h"
#include <QtGui/QApplication>
#include "Utils/XML/xmlserializer.h"
#include "Utils/XML/xmlserializable.h"
#include "Utils/XML/xmldeserializer.h"
#include "Utils/XML/xmlserializablefactory.h"
#include <QTest>
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
class MyTest : public QObject
{
	Q_OBJECT
public:
public slots:
	void test1();
};

void MyTest::test1()
{
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
}
QTEST_APPLESS_MAIN(MyTest)
#include "moc_test.cpp"