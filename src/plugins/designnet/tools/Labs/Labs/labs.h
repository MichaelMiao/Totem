#ifndef LABS_H
#define LABS_H

#include "labs_global.h"
#include "extensionsystem/iplugin.h"


class AdaboostSVMProcessor;
class LABS_EXPORT Labs : public ExtensionSystem::IPlugin
{
	Q_OBJECT

public:

	virtual bool initialize(const QStringList &arguments, QString *errorString);
	virtual void extensionsInitialized();

protected:

	AdaboostSVMProcessor* m_pAdaboostSVM;
};

#endif // LABS_H
