#include "welcomeplugin.h"
#include <QtPlugin>
using namespace  ExtensionSystem;
namespace Welcome{
WelcomePlugin::WelcomePlugin()
{

}

WelcomePlugin::~WelcomePlugin()
{

}

bool WelcomePlugin::initialize( const QStringList &arguments, QString *errorMessage /*= 0*/ )
{
	return true;
}

void WelcomePlugin::extensionsInitialized()
{
	return ;
}

bool WelcomePlugin::delayedInitialize()
{
	return true;
}

IPlugin::ShutdownFlag WelcomePlugin::aboutToShutdown()
{
	return IPlugin::SynchronousShutdown;
}

}
Q_EXPORT_PLUGIN2(Welcome::WelcomePlugin, Welcome::WelcomePlugin)
