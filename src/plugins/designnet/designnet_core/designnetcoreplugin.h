#pragma once
#include "../../../libs/extensionsystem/iplugin.h"
#include "designnet_global.h"
#include "designnetmainwindow.h"


class DesignNetCorePlugin : public ExtensionSystem::IPlugin
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "com.totem.plugins.designnet.core")
	Q_INTERFACES(ExtensionSystem::IPlugin)
public:

	DesignNetCorePlugin();
	~DesignNetCorePlugin();

	bool initialize(const QStringList &arguments, QString *errorString) override;
	void extensionsInitialized() override;

private:
	
};
