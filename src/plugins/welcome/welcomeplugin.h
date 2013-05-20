#ifndef WELCOMEPLUGIN_H
#define WELCOMEPLUGIN_H

#include "welcome_global.h"
#include "extensionsystem/iplugin.h"
namespace Welcome{

class WelcomePlugin : public ExtensionSystem::IPlugin
{
public:
	WelcomePlugin();
	~WelcomePlugin();
	bool initialize(const QStringList &arguments, QString *errorMessage = 0);
	void extensionsInitialized();
	bool delayedInitialize();
	IPlugin::ShutdownFlag aboutToShutdown();
private:

};

}

#endif // WELCOMEPLUGIN_H
