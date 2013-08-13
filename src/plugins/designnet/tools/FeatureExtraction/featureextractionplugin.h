#ifndef FEATUREEXTRACTIONPLUGIN_H
#define FEATUREEXTRACTIONPLUGIN_H

#include "featureextraction_global.h"
#include "extensionsystem/iplugin.h"
namespace FeatureExtraction{

class FeatureExtractionPluginPrivate;

class FEATUREEXTRACTION_EXPORT FeatureExtractionPlugin : public ExtensionSystem::IPlugin
{
	Q_OBJECT

public:
	FeatureExtractionPlugin(QObject *parent = 0);
	virtual ~FeatureExtractionPlugin();
	virtual bool initialize(const QStringList &arguments, QString *errorString);
	virtual void extensionsInitialized();
private:
	FeatureExtractionPluginPrivate *d;
};

}

#endif // FEATUREEXTRACTIONPLUGIN_H
