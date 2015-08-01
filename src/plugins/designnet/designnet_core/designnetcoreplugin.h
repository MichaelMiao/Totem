#ifndef DESIGNNETCOREPLUGIN_H
#define DESIGNNETCOREPLUGIN_H

#include "designnet_core_global.h"
#include "extensionsystem/iplugin.h"
namespace DesignNet{
class DesignNetCorePluginPrivate;


class DesignNetCorePlugin : public ExtensionSystem::IPlugin
{
public:

	Q_OBJECT
	Q_PLUGIN_METADATA(IID "com.michaelmiao.totem.DesignNetCorePlugin")
	Q_INTERFACES(ExtensionSystem::IPlugin)

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
