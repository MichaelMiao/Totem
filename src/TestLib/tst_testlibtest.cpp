#include <QString>
#include <QtTest>
#include <QtGui>
#include "extensionsystem/invoker.h"
#include "a.h"
using namespace GraphicsUI;
class TestLibTest : public QObject
{
    Q_OBJECT
    
public:
    TestLibTest();
    
private Q_SLOTS:
    void testCase1();
	void testCase2();
};

TestLibTest::TestLibTest()
{
}

void TestLibTest::testCase1()
{
// 	A a(0);
// 	int data = ExtensionSystem::Invoker<int>(&a, "add", 10, 20);
//    QVERIFY2(data == 30, "Failure");
}

void TestLibTest::testCase2()
{
	int iCount = 0;
	QApplication app(iCount, 0);
	A a(0);
	QStringList imageList;
	imageList << "F:\\1.jpg";
	imageList << "F:\\2.jpg";
	imageList << "F:\\3.jpg";
	a.m_autoView->pushImages(imageList);
	a.m_autoView->show();
	a.m_autoView->startToShow();
	app.exec();
}

QTEST_APPLESS_MAIN(TestLibTest)

#include "tst_testlibtest.moc"
