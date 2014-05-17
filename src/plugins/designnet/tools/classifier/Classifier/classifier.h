#ifndef CLASSIFIER_H
#define CLASSIFIER_H

#include "classifier_global.h"
#include "extensionsystem/iplugin.h"

class CLASSIFIER_EXPORT Classifier : public ExtensionSystem::IPlugin
{
	Q_OBJECT

public:

	Classifier();
	~Classifier();

	virtual bool initialize(const QStringList &arguments, QString *errorString);
	virtual void extensionsInitialized();
};
#endif // CLASSIFIER_H
