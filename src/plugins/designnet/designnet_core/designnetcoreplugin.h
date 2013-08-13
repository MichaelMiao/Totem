#ifndef DESIGNNETCOREPLUGIN_H
#define DESIGNNETCOREPLUGIN_H

#include "designnet_core_global.h"
#include "extensionsystem/iplugin.h"
namespace DesignNet{
class DesignNetCorePluginPrivate;
class DESIGNNET_CORE_EXPORT DesignNetCorePlugin : public ExtensionSystem::IPlugin
{
public:
	DesignNetCorePlugin();
	~DesignNetCorePlugin();
	bool initialize(const QStringList &arguments, QString *errorMessage = 0);
	void extensionsInitialized();
	bool delayedInitialize();
	ShutdownFlag aboutToShutdown();
public slots:
	void writeSettings();
private:
	DesignNetCorePluginPrivate *d;
};

}

#endif // DESIGNNETCOREPLUGIN_H
