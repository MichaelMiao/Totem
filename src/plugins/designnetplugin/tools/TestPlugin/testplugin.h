#ifndef TESTPLUGIN_H
#define TESTPLUGIN_H

#include "testplugin_global.h"
#include "extensionsystem/iplugin.h"
class TestLiFan;
class TESTPLUGIN_EXPORT TestPlugin : public ExtensionSystem::IPlugin
{
public:
	TestPlugin();
	~TestPlugin();
	virtual bool initialize(const QStringList &arguments, QString *errorString);
	virtual void extensionsInitialized();
private:
	TestLiFan* m_processor;
};

#endif // TESTPLUGIN_H
