#include "testplugin.h"
#include "testlifan.h"
#include <QtPlugin>
TestPlugin::TestPlugin()
{

}

TestPlugin::~TestPlugin()
{

}

bool TestPlugin::initialize( const QStringList &arguments, QString *errorString )
{
	return true;
}

void TestPlugin::extensionsInitialized()
{
	m_processor = new TestLiFan(0);
	addAutoReleasedObject(m_processor);
}

Q_EXPORT_PLUGIN2(TestPlugin, TestPlugin)