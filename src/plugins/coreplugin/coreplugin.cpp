#include "stdafx.h"
#include "coreplugin.h"
#include "icore.h"


CorePlugin::CorePlugin()
{
}

CorePlugin::~CorePlugin()
{
}

bool CorePlugin::initialize(const QStringList &arguments, QString *errorString)
{
	ICore::instance()->initialize();
	return true;
}

void CorePlugin::extensionsInitialized()
{
	ICore::instance()->extensionsInitialized();
}

ExtensionSystem::IPlugin::ShutdownFlag CorePlugin::aboutToShutdown()
{
	return SynchronousShutdown;
}
