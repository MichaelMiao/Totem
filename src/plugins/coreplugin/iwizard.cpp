#include "iwizard.h"
#include "extensionsystem/pluginmanager.h"
namespace Core{

QList<IWizard *> IWizard::wizardsOfKind(IWizard::WizardKind kind)
{
	// Filter all wizards
	const QList<IWizard*> allWizards = IWizard::allWizards();
	QList<IWizard*> rc;
	const QList<IWizard*>::const_iterator cend = allWizards.constEnd();
	for (QList<IWizard*>::const_iterator it = allWizards.constBegin(); it != cend; ++it)
		if (kind == (*it)->kind())
			rc.push_back(*it);
	return rc;
}

QList<IWizard *> IWizard::allWizards()
{
    return ExtensionSystem::PluginManager::instance()->getObjects<IWizard>();
}

}
