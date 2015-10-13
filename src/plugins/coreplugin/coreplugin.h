#pragma once
#ifndef COREPLUGIN_H
#define COREPLUGIN_H

#include "core_global.h"
#include "../../libs/extensionsystem/iplugin.h"

class CorePlugin : public ExtensionSystem::IPlugin
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "com.totem.plugins.core")
	Q_INTERFACES(ExtensionSystem::IPlugin)

public:

	bool initialize(const QStringList &arguments, QString *errorString) override;
	void extensionsInitialized() override;
	ShutdownFlag aboutToShutdown() override;

public:


	CorePlugin();
	~CorePlugin();

private:

};

#endif // COREPLUGIN_H
