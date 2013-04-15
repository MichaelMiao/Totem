#ifndef FLOWERFEATUREEXTRACTION_H
#define FLOWERFEATUREEXTRACTION_H

#include "flowerfeatureextraction_global.h"
#include "extensionsystem/iplugin.h"
namespace FeatureExtraction{

class FLOWERFEATUREEXTRACTION_EXPORT FlowerFeatureExtractionPlugin : public ExtensionSystem::IPlugin
{
	Q_OBJECT
public:
	FlowerFeatureExtractionPlugin(QObject *parent = 0);
	virtual ~FlowerFeatureExtractionPlugin();
	virtual bool initialize(const QStringList &arguments, QString *errorString);
	virtual void extensionsInitialized();
private:

};
}
#endif // FLOWERFEATUREEXTRACTION_H
