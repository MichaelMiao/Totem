#include "featureextractionplugin.h"
#include "glcmblock.h"
#include <QtPlugin>
namespace FeatureExtraction{
class FeatureExtractionPluginPrivate{
public:
	GLCMBlock *m_glcmBlock;
};
FeatureExtractionPlugin::FeatureExtractionPlugin(QObject *parent):
	d(new FeatureExtractionPluginPrivate())
{
	
}

FeatureExtractionPlugin::~FeatureExtractionPlugin()
{
	if(d)
	{
		delete d;
		d = 0;
	}
}

bool FeatureExtractionPlugin::initialize( const QStringList &arguments, QString *errorString )
{
	return true;
}

void FeatureExtractionPlugin::extensionsInitialized()
{
	d->m_glcmBlock = new GLCMBlock(0);
	addAutoReleasedObject(d->m_glcmBlock);
}
}

Q_EXPORT_PLUGIN2(FeatureExtraction::FeatureExtractionPlugin, FeatureExtraction::FeatureExtractionPlugin)