#include "featureextractionplugin.h"
#include "glcmblock.h"
#include "centroid.h"
#include <QtPlugin>
namespace FeatureExtraction{
class FeatureExtractionPluginPrivate{
public:
	GLCMBlock *	m_glcmBlock;
	Centroid *	m_centroid;
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
	d->m_glcmBlock	= new GLCMBlock(0);
	d->m_centroid	= new Centroid(0);
	addAutoReleasedObject(d->m_glcmBlock);
	addAutoReleasedObject(d->m_centroid);
}
}

Q_EXPORT_PLUGIN2(FeatureExtraction::FeatureExtractionPlugin, FeatureExtraction::FeatureExtractionPlugin)