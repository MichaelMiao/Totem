#ifndef FLOWERFEATUREEXTRACTION_H
#define FLOWERFEATUREEXTRACTION_H

#include "flowerfeatureextraction_global.h"
#include "extensionsystem/iplugin.h"
namespace FlowerFeatureExtraction{
class HSColorFeature;
class PolorGLCMBlock;
class FLOWERFEATUREEXTRACTION_EXPORT FlowerFeatureExtractionPlugin : public ExtensionSystem::IPlugin
{
	Q_OBJECT
public:
	FlowerFeatureExtractionPlugin(QObject *parent = 0);
	virtual ~FlowerFeatureExtractionPlugin();
	virtual bool initialize(const QStringList &arguments, QString *errorString);
	virtual void extensionsInitialized();
private:
	HSColorFeature*		m_featureProcessor;
	PolorGLCMBlock*		m_polorGLCM;
};
}
#endif // FLOWERFEATUREEXTRACTION_H
