#include "designnetprojectexplorer.h"
#include "coreplugin/actionmanager/actionmanager.h"
#include <QtPlugin>
using namespace Core;
using namespace ExtensionSystem;
namespace DesignNet{

DesignnetProjectExplorer::DesignnetProjectExplorer()
{

}

DesignnetProjectExplorer::~DesignnetProjectExplorer()
{

}

bool DesignnetProjectExplorer::initialize( const QStringList &arguments, QString *errorMessage /*= 0*/ )
{
	return true;
}

void DesignnetProjectExplorer::extensionsInitialized()
{

}

bool DesignnetProjectExplorer::delayedInitialize()
{
	return true;
}

}// namespace DesignNet

Q_EXPORT_PLUGIN2(DesignNet::DesignnetProjectExplorer, DesignNet::DesignnetProjectExplorer)