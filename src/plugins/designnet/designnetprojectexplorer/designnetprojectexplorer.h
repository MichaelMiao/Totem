#ifndef DESIGNNETPROJECTEXPLORER_H
#define DESIGNNETPROJECTEXPLORER_H

#include "designnetprojectexplorer_global.h"
#include "extensionsystem/iplugin.h"
namespace DesignNet{

class DESIGNNETPROJECTEXPLORER_EXPORT DesignnetProjectExplorer  : public ExtensionSystem::IPlugin
{
public:
	DesignnetProjectExplorer();
	~DesignnetProjectExplorer();
	bool initialize(const QStringList &arguments, QString *errorMessage = 0);
	void extensionsInitialized();
	bool delayedInitialize();
private:

};
};


#endif // DESIGNNETPROJECTEXPLORER_H
